/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "VideoProcessing.h"

#include "Utilities.h"
#include "Config.h"
#include "FFMPEGProcessing.h"
#include "OpenFaceProcessing.h"
#include "EyeLikeProcessing.h"
#include "SystemHelper.h"
#include "StreamingFrameSource.h"

#include <boost/filesystem.hpp>
#include <tbb/tbb.h>
#include <opencv2/opencv.hpp>

#include <iterator>
#include <vector>
#include <thread>
#include <tuple>
#include <signal.h>
#include <future>


#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

class OpenImages
{
public:
    std::vector<cv::Mat>* images;
    const std::string* framesFormat;
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
        for (size_t i = r.begin(); i != r.end(); i++)
        {
            char buffer[128];
            snprintf(buffer, 128, framesFormat->c_str(), i);
            std::string input(buffer);
            (*images)[i-1] = OpenFaceProcessing::openImage(input);
        }
    }
};

class SaveImages
{
public:
    std::vector<FrameData>* frameData;
    const std::string* framesFormat;
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
        for (size_t i = r.begin(); i != r.end(); i++)
        {
            char buffer[128];
            snprintf(buffer, 128, framesFormat->c_str(), i);
            std::string output(buffer);
            OpenFaceProcessing::saveImage(output, (*frameData)[i-1].outputImage);
        }
    }
};

void processFrame(const cv::Mat& input, 
                  FrameData& output, 
                  const VideoMetadata& metadata, 
                  LandmarkDetector::CLNF& model,
                  cv::CascadeClassifier classifier)
{
    cv::Mat_<uchar> grayImage;
    Utilities::ConvertToGrayscale_8bit(input, grayImage);
    
    // Scale the image to be processed to about 320x240
    int scaleFactor = (grayImage.size().width / 320);
    cv::resize(grayImage, grayImage, grayImage.size() / scaleFactor, 0, 0, cv::INTER_LINEAR);
    
    auto pupilsFuture = std::async(std::launch::async, [&]()
    {
        return EyeLikeProcessing::detectPupils(grayImage, classifier);
    });
    
    output.dataPoints = 
        OpenFaceProcessing::extractFaceDataPoints(grayImage, metadata, model);
    output.headPose = 
        OpenFaceProcessing::extractHeadPose(model, metadata);
    output.delaunayTriangles = 
        OpenFaceProcessing::getDelaunayTriangles(output.dataPoints, metadata);

    Config::output.outputFrameData(output);
    
    OpenFaceProcessing::applyFaceDataPointsToImage(output.outputImage, output.dataPoints, metadata, scaleFactor);
    OpenFaceProcessing::applyDelaunayTrianlgesToImage(output.outputImage, output.delaunayTriangles, metadata, scaleFactor);
    EyeLikeProcessing::applyEyeCentersToImage(output.outputImage, pupilsFuture.get(), scaleFactor);
}

void processSetOfFrames(std::vector<cv::Mat>::const_iterator inputIt,
                        std::vector<cv::Mat>::const_iterator inputItEnd,
                        std::vector<FrameData>::iterator outputIt,
                        std::vector<FrameData>::iterator outputItEnd,
                        int step,
                        const VideoMetadata& metadata,
                        LandmarkDetector::CLNF& model,
                        cv::CascadeClassifier& classifier)
{
    int processed = 0;
    while (true)
    {
        processFrame(*inputIt, *outputIt, metadata, model, classifier);
        processed++;
        for (int i = 0; i < step; i++)
        {
            ++inputIt;
            ++outputIt;
            
            if (inputIt == inputItEnd || outputIt == outputItEnd)
            {
                return;
            }
        }
    }
}

void processVideo(const std::string& framesFormat,
                  const std::string& processedFormat,
                  const VideoMetadata& metadata)
{
    Utilities::uint64 tStart, tEnd;
    
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    tStart = Utilities::GetTimeMs64();
    
    LandmarkDetector::FaceModelParameters detParameters(args);
    LandmarkDetector::CLNF clnfModel1(detParameters.model_location);
    LandmarkDetector::CLNF clnfModel2(clnfModel1);
    cv::CascadeClassifier classifier1(EyeLikeProcessing::createClassifier());
    cv::CascadeClassifier classifier2(EyeLikeProcessing::createClassifier());
    
    tEnd = Utilities::GetTimeMs64();
    
    Config::output.log("OpenFace Initialization Took: " 
                        + std::to_string(tEnd-tStart) + "ms\n",
                        OutputWriter::LogLevel::Info);
    
    int imageCount = metadata.numFrames;
    
    std::vector<cv::Mat> images((size_t)imageCount);
    
    OpenImages openImagesLoop;
    openImagesLoop.framesFormat = &framesFormat;
    openImagesLoop.images  = &images;
    tbb::parallel_for(tbb::blocked_range<size_t>(1, imageCount + 1), openImagesLoop);
    
    std::vector<FrameData> frameData((size_t)imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        frameData[i].frameNumber = i;
        frameData[i].outputImage = cv::Mat(images[i]);
    }
    
    tStart = Utilities::GetTimeMs64();
    
    std::thread t1(processSetOfFrames, images.cbegin(), images.cend(), frameData.begin(), frameData.end(), 2, std::cref(metadata), std::ref(clnfModel1), std::ref(classifier1));
    std::thread t2(processSetOfFrames, images.cbegin()+1, images.cend(), frameData.begin()+1, frameData.end(), 2, std::cref(metadata), std::ref(clnfModel2), std::ref(classifier2));
    
    t1.join();
    t2.join();

    tEnd = Utilities::GetTimeMs64();

    Config::output.log("Frame Processing Took: " 
                        + std::to_string(tEnd-tStart) + "ms ("
                        + std::to_string((tEnd - tStart) / imageCount) 
                        + "ms per frame)\n",
                        OutputWriter::LogLevel::Info);
    
    SaveImages saveImagesLoop;
    saveImagesLoop.framesFormat = &processedFormat;
    saveImagesLoop.frameData = &frameData;
    tbb::parallel_for(tbb::blocked_range<size_t>(1, imageCount + 1), saveImagesLoop);
}

// Continually processes frames using the given input and output
// functions. Intended to be called in a separate thread.
void processVideoStreamFrames(const VideoMetadata& metadata,
                              LandmarkDetector::CLNF& model,
                              cv::CascadeClassifier& classifier,
                              std::function<cv::Mat()> input,
                              std::function<void(const FrameData&)> output)
{
    while (true)
    {
        cv::Mat image = input();
        
        FrameData data;
        data.outputImage = cv::Mat(image);
        
        processFrame(image, data, metadata, model, classifier);
        
        output(data);
    }
}

// Processed the video stream from the given pipe
void processVideoStream(const std::string& inPipe,
                        const std::string& outputStream,
                        const VideoMetadata& metadata)
{
    Utilities::uint64 tStart, tEnd;
    
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    tStart = Utilities::GetTimeMs64();
    
    LandmarkDetector::FaceModelParameters detParameters(args);
    LandmarkDetector::CLNF clnfModel1(detParameters.model_location);
    cv::CascadeClassifier classifier(EyeLikeProcessing::createClassifier());
    
    tEnd = Utilities::GetTimeMs64();
    
    Config::output.log("OpenFace Initialization Took: " 
                        + std::to_string(tEnd-tStart) + "ms\n",
                        OutputWriter::LogLevel::Info);
    
    tStart = Utilities::GetTimeMs64();
    
    const std::string outPipeName = "cvprocessor-out";
    createFIFO(outPipeName);
    
    // Start up read end of pipe
    FFMPEGProcessing::outputFramesToStreamFromPipe(
        outputStream,
        outPipeName,
        metadata);
    
    int fifo;
    // Open pipe for outputting images. Waits for reader.
    if ((fifo = open(outPipeName.c_str(), O_WRONLY)) < 0) 
    {
        Config::output.log("Unable to open output pipe\n",
                           OutputWriter::LogLevel::Debug);
    }

    StreamingFrameSource streamFrameSource(metadata);
    streamFrameSource.openFIFO(inPipe);
    
    Utilities::uint64 tFrameStart, tFrameEnd;
    tFrameStart = Utilities::GetTimeMs64();
    
    processVideoStreamFrames(metadata, clnfModel1, classifier,
    [&]() -> cv::Mat
    {
        return streamFrameSource.getLatestFrame();
    },
    [&](const FrameData& data)
    {
        // TODO: check data.outputImage.continuous or whatever its called
        if (write(fifo, data.outputImage.data, metadata.width*metadata.height*3) < 0)
        {
            Config::output.log("Error when writing to pipe\n",
                               OutputWriter::LogLevel::Debug);
        }
        
        tFrameEnd = Utilities::GetTimeMs64();
        Config::output.log("Frame Took: " 
                        + std::to_string(tFrameEnd-tFrameStart) + "ms\n",
                        OutputWriter::LogLevel::Info);
        tFrameStart = tFrameEnd;
    });

    // Close the output pipe
    close(fifo);
    
    tEnd = Utilities::GetTimeMs64();
    
    Config::output.log("Frame Processing Took: " 
                        + std::to_string(tEnd-tStart) + "ms\n",
                        OutputWriter::LogLevel::Info);
}

void VideoProcessing::processVideo(const std::string& inputFile, 
                                   const std::string& outputFile)
{
    VideoMetadata metadata = FFMPEGProcessing::extractMetadata(inputFile);
    Config::output.outputMetadata(metadata);

    FFMPEGProcessing::extractFrames(inputFile, 
        "frames/out%d.png", metadata);
        
    processVideo("frames/out%d.png", "processed/out%d.png", metadata);
        
    FFMPEGProcessing::combineFrames("processed/out%d.png", 
        outputFile, metadata);
}

void VideoProcessing::processVideoStream(const std::string& inputStream,
                                         const std::string& outputStream)
{
    VideoMetadata metadata = FFMPEGProcessing::extractMetadataFromStream(Config::videoStream);
    Config::output.outputMetadata(metadata);
    
    // limit the frame rate to 20 FPS
    // even if the stream is coming in faster
    //if ((metadata.frameRateNum / (float)metadata.frameRateDenom) > 20)
    {
        metadata.frameRateNum = 20;
        metadata.frameRateDenom = 1;
    }
    
    const std::string inPipeName = "cvprocessor-in";
    createFIFO(inPipeName);
    
    int extractionProcessID = FFMPEGProcessing::extractFramesFromStream(Config::videoStream, 
        inPipeName, metadata);

    processVideoStream(inPipeName, outputStream, metadata);
}