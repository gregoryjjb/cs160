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

#include <OpenFace/LandmarkDetectorModel.h>

#include "VideoMetadata.h"
#include "FFMPEGProcessing.h"
#include "OpenFaceProcessing.h"

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        std::string inputFile = argv[1];
        
        VideoMetadata metadata = 
            FFMPEGProcessing::extractMetadata(inputFile);
        FFMPEGProcessing::extractFrames(inputFile, "frames/out%d.png", metadata);
        
        
        std::vector<std::string> args{};
        args.push_back("-q");
        args.push_back("-wild");
        
        LandmarkDetector::FaceModelParameters detParameters(args);
        LandmarkDetector::CLNF clnfModel(detParameters.model_location);
        
        for (int i = 1; i < metadata.numFrames + 1; i++)
        {
            std::stringstream istream;
            istream << "frames/out" << i << ".png";
            std::string input = istream.str();
            
            std::stringstream ostream;
            ostream << "processed/out" << i << ".png";
            std::string output = ostream.str();
            
            OpenFaceProcessing::FaceDataPointsRecord dataPoints 
                = OpenFaceProcessing::extractFaceDataPoints(input, metadata, clnfModel);
            
            OpenFaceProcessing::applyFaceDataPointsToImage(input, output, dataPoints, metadata);
            
            std::cout << "Finished " << i << std::endl;
        }
        
        FFMPEGProcessing::combineFrames("processed/out%d.png", "processed.mp4", metadata);
        
        std::cout << "Extracted the following metadata: " << metadata.width << std::endl
            << " " << metadata.height << std::endl
            << " " << metadata.numFrames << std::endl
            << " " << metadata.frameRateNum << std::endl
            << " " << metadata.frameRateDenom << std::endl;

    }
    
    return 0;
}

