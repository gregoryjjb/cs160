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

#include "VideoMetadata.h"
#include "FFMPEGProcessing.h"
#include "OpenFaceProcessing.h"
#include "Utilities.h"

void processFrame(const cv::Mat& input, 
                  cv::Mat& output, 
                  const VideoMetadata& metadata, 
                  LandmarkDetector::CLNF& model)
{
    cv::Mat_<uchar> grayImage;
    Utilities::ConvertToGrayscale_8bit(input, grayImage);
    
    OpenFaceProcessing::FaceDataPointsRecord dataPoints
                = OpenFaceProcessing::extractFaceDataPoints(grayImage, metadata, model);
    
    OpenFaceProcessing::applyFaceDataPointsToImage(input, output, dataPoints, metadata);
    
    std::vector<cv::Vec6f> triangles = 
        OpenFaceProcessing::getDelaunayTriangles(dataPoints, metadata);
    
    OpenFaceProcessing::applyDelaunayTrianlgesToImage(output, triangles, metadata);
}

void processSetOfFrames(std::vector<cv::Mat>::const_iterator inputIt,
                        std::vector<cv::Mat>::const_iterator inputItEnd,
                        std::vector<cv::Mat>::iterator outputIt,
                        std::vector<cv::Mat>::iterator outputItEnd,
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
                std::cout << "Thread processed " << processed << std::endl;
                return;
            }
        }
    }
}

void processVideo(const std::string& framesFormat,
                  const std::string& processedFormat,
                  const VideoMetadata& metadata)
{
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    LandmarkDetector::FaceModelParameters detParameters(args);
    LandmarkDetector::CLNF clnfModel1(detParameters.model_location);
    LandmarkDetector::CLNF clnfModel2(clnfModel1);
    
    int imageCount = metadata.numFrames;
    
    std::vector<cv::Mat> images((size_t)imageCount);
    std::vector<cv::Mat> processedImages((size_t)imageCount);
    
    // Open all images of the video
    for (int i = 1; i < imageCount + 1; i++)
    {
        char buffer[128];
        snprintf(buffer, 128, framesFormat.c_str(), i);
        std::string input(buffer);
        images[i-1] = OpenFaceProcessing::openImage(input);
    }
    
    Utilities::uint64 tStart = Utilities::GetTimeMs64();
    
    std::thread t1(processSetOfFrames, images.cbegin(), images.cend(), processedImages.begin(), processedImages.end(), 2, std::cref(metadata), std::ref(clnfModel1));
    std::thread t2(processSetOfFrames, images.cbegin()+1, images.cend(), processedImages.begin()+1, processedImages.end(), 2, std::cref(metadata), std::ref(clnfModel2));
    
    t1.join();
    t2.join();
    
    Utilities::uint64 tEnd = Utilities::GetTimeMs64();
    
    std::cout << "Processing Took: " << (tEnd - tStart) << "ms" << std::endl;

    // Save all processed images
    for (int i = 1; i < imageCount + 1; i++)
    {
        char buffer[128];
        snprintf(buffer, 128, processedFormat.c_str(), i);
        std::string output(buffer);
        OpenFaceProcessing::saveImage(output, processedImages[i-1]);
    }
}

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        std::string inputFile = argv[1];

        VideoMetadata metadata =
            FFMPEGProcessing::extractMetadata(inputFile);
        
        std::cout << "Extracted the following metadata: " << metadata.width << std::endl
            << " " << metadata.height << std::endl
            << " " << metadata.numFrames << std::endl
            << " " << metadata.frameRateNum << std::endl
            << " " << metadata.frameRateDenom << std::endl;
        
        FFMPEGProcessing::extractFrames(inputFile, "frames/out%d.png", metadata);

        processVideo("frames/out%d.png", "processed/out%d.png", metadata);

        FFMPEGProcessing::combineFrames("processed/out%d.png", "processed.mp4", metadata);
    }

    return 0;
}
