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
                  LandmarkDetector::CLNF& model)
{
    cv::Mat_<uchar> grayImage;
    Utilities::ConvertToGrayscale_8bit(input, grayImage);
    
    // Scale the image to be processed to about 640x480
    int scaleFactor = (grayImage.size().width / 480);
    cv::resize(grayImage, grayImage, grayImage.size() / scaleFactor, 0, 0, cv::INTER_LINEAR);

    output.dataPoints = 
        OpenFaceProcessing::extractFaceDataPoints(grayImage, metadata, model);
    output.headPose = 
        OpenFaceProcessing::extractHeadPose(model, metadata);
    output.delaunayTriangles = 
        OpenFaceProcessing::getDelaunayTriangles(output.dataPoints, metadata);
    
    // Use the detected landmarks to calculate face and eye regions
    cv::Rect rect(Utilities::GetBoundingRect(output.dataPoints.landmarks));
    cv::Rect leftPupil(Utilities::GetBoundingRect(output.dataPoints.landmarks, 36, 41));
    cv::Rect rightPupil(Utilities::GetBoundingRect(output.dataPoints.landmarks, 42, 47));
    
    // Transform pupil regions to face coordinates
    leftPupil.x -= rect.x;
    leftPupil.y -= rect.y;
    rightPupil.x -= rect.x;
    rightPupil.y -= rect.y;
    
    // If there is no face
    if (rect.width == 0 || rect.height == 0)
    {
        output.pupils = std::make_tuple(cv::Point(0,0), cv::Point(0,0));
    }
    else
    {
        output.pupils = 
            EyeLikeProcessing::detectPupils(grayImage, rect, leftPupil, rightPupil);
    }
    
    OpenFaceProcessing::applyFaceDataPointsToImage(output.outputImage, output.dataPoints, metadata, scaleFactor);
    OpenFaceProcessing::applyDelaunayTrianlgesToImage(output.outputImage, output.delaunayTriangles, metadata, scaleFactor);

    EyeLikeProcessing::applyEyeCentersToImage(output.outputImage, output.pupils, scaleFactor);
    
    Config::output.outputFrameData(output);
}

void processSetOfFrames(std::vector<cv::Mat>::const_iterator inputIt,
                        std::vector<cv::Mat>::const_iterator inputItEnd,
                        std::vector<FrameData>::iterator outputIt,
                        std::vector<FrameData>::iterator outputItEnd,
                        int step,
                        const VideoMetadata& metadata,
                        LandmarkDetector::CLNF& model)
{
    int processed = 0;
    while (true)
    {
        processFrame(*inputIt, *outputIt, metadata, model);
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
    LandmarkDetector::CLNF clnfModel(detParameters.model_location);
    
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

    processSetOfFrames(images.cbegin(), images.cend(), 
                    frameData.begin(), frameData.end(), 
                    1, metadata, clnfModel);

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
                              std::function<cv::Mat()> input,
                              std::function<void(const FrameData&)> output)
{
    int i = 0;
    while (true)
    {
        cv::Mat image = input();
        
        FrameData data;
        data.frameNumber = i++;
        data.outputImage = cv::Mat(image);
        
        processFrame(image, data, metadata, model);
        
        output(data);
    }
}

// Processed the video stream from the given pipe
void processVideoStream(const std::string& inPipe,
                        const VideoMetadata& metadata)
{
    Utilities::uint64 tStart, tEnd;
    
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    tStart = Utilities::GetTimeMs64();
    
    LandmarkDetector::FaceModelParameters detParameters(args);
    
    // Disable std::out during model initialization
    // to avoid spamming the console which is used 
    // for output when streaming
    Config::output.disableOtherStdOutStreams();
    LandmarkDetector::CLNF clnfModel1(detParameters.model_location);
    Config::output.enableOtherStdOutStreams();
    
    tEnd = Utilities::GetTimeMs64();
    
    Config::output.log("OpenFace Initialization Took: " 
                        + std::to_string(tEnd-tStart) + "ms\n",
                        OutputWriter::LogLevel::Info);
    
    tStart = Utilities::GetTimeMs64();
    
    const std::string outPipeName = "cvprocessor-out";
    createFIFO(outPipeName);
    
    // Start up read end of pipe
    FFMPEGProcessing::outputFramesToStdOutFromPipe(
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
    
    processVideoStreamFrames(metadata, clnfModel1,
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

void VideoProcessing::processVideoStream(const std::string& inputStream)
{
    VideoMetadata metadata;
    if (Config::execMode == Config::ExecutionMode::StandardIO)
    {
        int sep = Config::cmdFrameRate.find('/');
        int num = std::stoi(Config::cmdFrameRate.substr(0,sep));
        int denom = std::stoi(Config::cmdFrameRate.substr(sep + 1));
        metadata = VideoMetadata(Config::cmdWidth, Config::cmdHeight, -1, num, denom);
    }
    else if (Config::execMode == Config::ExecutionMode::VideoStream)
    {
        metadata = FFMPEGProcessing::extractMetadataFromStream(inputStream);
    }
    else
    {
        Config::output.log("Unrecognized execution mode", OutputWriter::LogLevel::Debug);
        exit(1);
    }
    
    Config::output.outputMetadata(metadata);

    // limit the frame rate to 24 FPS
    // even if the stream is coming in faster
    //if ((metadata.frameRateNum / (float)metadata.frameRateDenom) > 20)
    {
        metadata.frameRateNum = 24;
        metadata.frameRateDenom = 1;
    }
    
    // TODO: unique names
    const std::string framePipe = "cvprocessor-frames";
    createFIFO(framePipe);
    
    int extractionProcessID = FFMPEGProcessing::extractFramesFromStream(inputStream, 
        framePipe, metadata);

    processVideoStream(framePipe, metadata);
}
