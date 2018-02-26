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

#include <OpenFace/LandmarkDetectorModel.h>

#include "VideoMetadata.h"
#include "FFMPEGProcessing.h"
#include "OpenFaceProcessing.h"
#include "Utilities.h"

void processFrame(const std::string& input, 
                  const std::string& output, 
                  const VideoMetadata& metadata, 
                  const LandmarkDetector::CLNF& originalModel)
{
    std::cout << "Starting " << input << std::endl;
    
    cv::Mat image = OpenFaceProcessing::openImage(input);
    cv::Mat_<uchar> grayImage;
    Utilities::ConvertToGrayscale_8bit(image, grayImage);
    
    OpenFaceProcessing::FaceDataPointsRecord dataPoints
                = OpenFaceProcessing::extractFaceDataPoints(grayImage, metadata, originalModel);
    
    cv::Mat processedImage;
    OpenFaceProcessing::applyFaceDataPointsToImage(image, processedImage, dataPoints, metadata);
    
    OpenFaceProcessing::saveImage(output, processedImage);
    
    std::cout << "Finished " << input << std::endl;
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

    std::thread threads[metadata.numFrames];
    
    for (int i = 1; i < metadata.numFrames + 1; i++)
    {
        char buffer[128];
        // TODO: guard against buffer overflow
        sprintf(buffer, framesFormat.c_str(), i);
        std::string input(buffer);
        
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, processedFormat.c_str(), i);
        std::string output(buffer);
        
        //std::cout << "Creating " << input << std::endl;
        
        threads[i-1] = std::thread(processFrame, input, output, std::cref(metadata), std::cref(clnfModel));
    }
    
    for (int i = 0; i < metadata.numFrames; i++)
    {
        threads[i].join();
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
