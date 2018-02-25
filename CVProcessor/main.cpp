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
#include "VideoMetadata.h"
#include "FFMPEGProcessing.h"

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        std::string inputFile = argv[1];
        VideoMetadata metadata = 
            FFMPEGProcessing::extractMetadata(inputFile);
        FFMPEGProcessing::extractFrames(inputFile, "frames/out%d.png", metadata);
        FFMPEGProcessing::combineFrames("frames/out%d.png", "output.mp4", metadata);
        
        std::cout << "Extracted the following metadata: " << metadata.width << std::endl
            << " " << metadata.height << std::endl
            << " " << metadata.numFrames << std::endl
            << " " << metadata.frameRateNum << std::endl      
            << " " << metadata.frameRateDenom << std::endl;

    }
    return 0;
}

