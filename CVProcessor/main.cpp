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

void processVideo(const std::string& framesFormat,
                  const std::string& processedFormat,
                  const VideoMetadata& metadata)
{
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    LandmarkDetector::FaceModelParameters detParameters(args);
    LandmarkDetector::CLNF clnfModel(detParameters.model_location);
    
    int imageCount = metadata.numFrames;
    
    cv::Mat images[imageCount];
    cv::Mat processedImages[imageCount];
    
    // Open all images of the video
    for (int i = 1; i < imageCount + 1; i++)
    {
        char buffer[128];
        snprintf(buffer, 128, framesFormat.c_str(), i);
        std::string input(buffer);
        images[i-1] = OpenFaceProcessing::openImage(input);
    }
    
    for (int i = 1; i < imageCount + 1; i++)
    {
        processFrame(images[i-1], processedImages[i-1], metadata, clnfModel);
    }
    
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
