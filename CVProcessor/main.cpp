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
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <array>
#include "VideoMetadata.h"

/*
 Executes the given command and returns the output of that command
 */
std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

/*
 Extracts various metadata from a video at the given path
 */
VideoMetadata extractMetadata(const std::string& targetVideo)
{
    const char* format =
        "ffprobe -v error -count_frames -select_streams v:0 "
        "-show_entries stream=avg_frame_rate,nb_read_frames,width,height "
        "-of default=noprint_wrappers=1:nokey=1 %s";
    // TODO: Validate that we wont overflow buffer
    char buffer[256];
    sprintf(buffer, format, targetVideo.c_str());
    
    std::istringstream result;
    result.str(exec(std::string(buffer)));
    
    std::string widthStr, heightStr, avgFramesStr, frameCountStr;
    std::getline(result, widthStr);    
    std::getline(result, heightStr);
    std::getline(result, avgFramesStr);
    std::getline(result, frameCountStr);

    int width = std::stoi(widthStr);
    int height = std::stoi(heightStr);

    int sep = avgFramesStr.find('/');
    int num = std::stoi(avgFramesStr.substr(0,sep));
    int denom = std::stoi(avgFramesStr.substr(sep + 1));

    int frameCount = std::stoi(frameCountStr);
    
    return VideoMetadata(width, height, frameCount, num, denom);
}

void splitFrames(const std::string& targetVideo, 
                 const std::string& outputFolder, 
                 const VideoMetadata& metadata)
{
    // TODO: create outputFolder if needed
    // TODO: Validate that we wont overflow buffer
    const char* format = "ffmpeg -i %s -vf fps=%d/%d %s/out%s.png 2>&1";
    char buffer[256];
    sprintf(buffer, format, targetVideo.c_str(), metadata.frameRateNum, 
            metadata.frameRateDenom, outputFolder.c_str(), "%d");
    exec(std::string(buffer));
}

/*
 * 
 */
int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        std::string inputFile = argv[1];
        VideoMetadata metadata = extractMetadata(inputFile);
        splitFrames(inputFile, "frames", metadata);
        
        std::cout << "Extracted the following metadata: " << metadata.width << std::endl
            << " " << metadata.height << std::endl
            << " " << metadata.numFrames << std::endl
            << " " << metadata.frameRateNum << std::endl      
            << " " << metadata.frameRateDenom << std::endl;

    }
    return 0;
}

