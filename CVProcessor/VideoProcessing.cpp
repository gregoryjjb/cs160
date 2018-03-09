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

#include <vector>
#include <thread>
#include <signal.h>
#include <future>
#include <tbb/tbb.h>

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
    
    auto pupilsFuture = std::async(std::launch::async, [&]()
    {
        return EyeLikeProcessing::detectPupils(grayImage);
    });
    
    output.dataPoints = 
        OpenFaceProcessing::extractFaceDataPoints(grayImage, metadata, model);
    output.headPose = 
        OpenFaceProcessing::extractHeadPose(model, metadata);
    output.delaunayTriangles = 
        OpenFaceProcessing::getDelaunayTriangles(output.dataPoints, metadata);

    Config::output.outputFrameData(output);
    
    OpenFaceProcessing::applyFaceDataPointsToImage(output.outputImage, output.dataPoints, metadata);
    OpenFaceProcessing::applyDelaunayTrianlgesToImage(output.outputImage, output.delaunayTriangles, metadata);
    EyeLikeProcessing::applyEyeCentersToImage(output.outputImage, pupilsFuture.get());
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
    LandmarkDetector::CLNF clnfModel1(detParameters.model_location);
    LandmarkDetector::CLNF clnfModel2(clnfModel1);
    
    tEnd = Utilities::GetTimeMs64();
    
    std::cout << "OpenFace Initialization Took: " << (tEnd - tStart) << "ms" << std::endl;
    
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
    
    std::thread t1(processSetOfFrames, images.cbegin(), images.cend(), frameData.begin(), frameData.end(), 2, std::cref(metadata), std::ref(clnfModel1));
    std::thread t2(processSetOfFrames, images.cbegin()+1, images.cend(), frameData.begin()+1, frameData.end(), 2, std::cref(metadata), std::ref(clnfModel2));
    
    t1.join();
    t2.join();

    tEnd = Utilities::GetTimeMs64();
    
    std::cout << "Frame Processing(w/o IO) Took: " << (tEnd - tStart) << "ms (" 
        << (tEnd - tStart) / imageCount << "ms per frame)" << std::endl;

    SaveImages saveImagesLoop;
    saveImagesLoop.framesFormat = &processedFormat;
    saveImagesLoop.frameData = &frameData;
    tbb::parallel_for(tbb::blocked_range<size_t>(1, imageCount + 1), saveImagesLoop);
}

void VideoProcessing::processVideo(const std::string& inputFile, 
                                   const std::string& outputFile)
{
    VideoMetadata metadata = FFMPEGProcessing::extractMetadata(outputFile);
    Config::output.outputMetadata(metadata);

    FFMPEGProcessing::extractFrames(outputFile, 
        "frames/out%d.png", metadata);
        
    processVideo("frames/out%d.png", "processed/out%d.png", metadata);
        
    FFMPEGProcessing::combineFrames("processed/out%d.png", 
        outputFile, metadata);
}

void VideoProcessing::processVideoStream(const std::string& inputStream)
{
    VideoMetadata metadata = FFMPEGProcessing::extractMetadataFromStream(Config::videoStream);
    Config::output.outputMetadata(metadata);
        
    int extractionProcessID = FFMPEGProcessing::extractFramesFromStream(Config::videoStream, 
        "frames/out%d.png", metadata);

    sleep(5);
    kill(extractionProcessID, 15);
}