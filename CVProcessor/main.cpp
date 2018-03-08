/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dean
 *
 * Created on February 23, 2018, 12:22 PM
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>

#include <tbb/tbb.h>

#include "Config.h"
#include "FrameData.h"
#include "VideoMetadata.h"
#include "FFMPEGProcessing.h"
#include "OpenFaceProcessing.h"
#include "EyeLikeProcessing.h"
#include "Utilities.h"

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

// Returns cmd line arguments as a vector of strings
// for convenience
std::vector<std::string> parseArguments(int argc, char** argv)
{
    std::vector<string> args;
    for (int i = 0; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
    return args;
}

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        std::string inputFile = argv[1];

        Utilities::uint64 tStart = Utilities::GetTimeMs64();
        
        VideoMetadata metadata =
            FFMPEGProcessing::extractMetadata(inputFile);
        
        Config::output.disableOtherStdOutStreams();
        Config::output.outputMetadata(metadata);
        
        FFMPEGProcessing::extractFrames(inputFile, "frames/out%d.png", metadata);
        
        processVideo("frames/out%d.png", "processed/out%d.png", metadata);

        FFMPEGProcessing::combineFrames("processed/out%d.png", "processed.mp4", metadata);
        
        Utilities::uint64 tEnd = Utilities::GetTimeMs64();
        
        std::cout << "Full Processing Took: " << (tEnd - tStart) << "ms" << std::endl;
    }

    return 0;
}
