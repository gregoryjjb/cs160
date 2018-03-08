/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include "SystemHelper.h"
#include "FFMPEGProcessing.h"

VideoMetadata FFMPEGProcessing::extractMetadata(const std::string& videoFile)
{
    const char* format =
        "ffprobe -v error -count_frames -select_streams v:0 "
        "-show_entries stream=avg_frame_rate,nb_read_frames,width,height "
        "-of default=noprint_wrappers=1:nokey=1 %s";
    // TODO: Validate that we wont overflow buffer
    char buffer[256];
    sprintf(buffer, format, videoFile.c_str());
    
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

void FFMPEGProcessing::extractFrames(const std::string& targetVideo, 
                 const std::string& outputFormat,
                 const VideoMetadata& metadata)
{
    // TODO: Validate that we wont overflow buffer
    const char* format = "ffmpeg -i %s -vf fps=%d/%d %s 2>&1";
    char buffer[256];
    sprintf(buffer, format, targetVideo.c_str(), metadata.frameRateNum, 
            metadata.frameRateDenom, outputFormat.c_str());
    exec(std::string(buffer));
}

void FFMPEGProcessing::combineFrames(const std::string& inputFormat,
                                     const std::string& outputName,
                                     const VideoMetadata& metadata)
{
    // TODO: parameterize the input frame names and encoding
    // TODO: Validate that we wont overflow buffer
    const char* format = 
        "ffmpeg -r %d/%d -start_number 1 -f image2 -i "
        "%s -c:v libx264 %s 2>&1 &";
    char buffer[256];
    sprintf(buffer, format, metadata.frameRateNum, 
            metadata.frameRateDenom, inputFormat.c_str(), outputName.c_str());
    exec(std::string(buffer));
}
