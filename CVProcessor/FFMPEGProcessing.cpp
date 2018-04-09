/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include <unistd.h>
#include "Config.h"
#include "SystemHelper.h"
#include "FFMPEGProcessing.h"

VideoMetadata FFMPEGProcessing::extractMetadata(const std::string& videoFile)
{
    const char* format =
        "ffprobe -v error -count_frames -select_streams v:0 "
        "-show_entries stream=avg_frame_rate,nb_read_frames,width,height "
        "-of default=noprint_wrappers=1:nokey=1 %s";

    char buffer[256];
    snprintf(buffer, 256, format, videoFile.c_str());
    
    std::istringstream result;
    result.str(execAndGetOutput(std::string(buffer)));
    
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

VideoMetadata FFMPEGProcessing::extractMetadataFromStream(const std::string& stream)
{
    const char* format =
        "ffprobe -v error -select_streams v:0 "
        "-show_entries stream=avg_frame_rate,,width,height "
        "-of default=noprint_wrappers=1:nokey=1 %s";

    char buffer[256];
    snprintf(buffer, 256, format, stream.c_str());
    
    std::istringstream result;
    result.str(execAndGetOutput(std::string(buffer)));
    
    std::string widthStr, heightStr, avgFramesStr;
    std::getline(result, widthStr);    
    std::getline(result, heightStr);
    std::getline(result, avgFramesStr);

    int width = std::stoi(widthStr);
    int height = std::stoi(heightStr);

    int sep = avgFramesStr.find('/');
    int num = std::stoi(avgFramesStr.substr(0,sep));
    int denom = std::stoi(avgFramesStr.substr(sep + 1));
    
    return VideoMetadata(width, height, -1, num, denom);
}

void FFMPEGProcessing::extractFrames(const std::string& targetVideo, 
                 const std::string& outputFormat,
                 const VideoMetadata& metadata)
{
    const char* format = "ffmpeg -i %s -vf fps=%d/%d %s 2>&1";
    char buffer[256];
    snprintf(buffer, 256, format, targetVideo.c_str(), metadata.frameRateNum, 
            metadata.frameRateDenom, outputFormat.c_str());
    execAndGetOutput(std::string(buffer));
}

int FFMPEGProcessing::extractFramesFromStream(const std::string& stream, 
                                               const std::string& outputFormat, 
                                               const VideoMetadata& metadata)
{
    int pid = fork();
    if (pid < 0)
    {
        Config::output.log("Error forking to process the incoming stream\n", 
                           OutputWriter::LogLevel::Debug);
    }
    else if (pid == 0) // Child
    {
        const char* format = 
        "ffmpeg -i %s -f rawvideo -vcodec rawvideo "
        "-video_size %dx%d -pix_fmt bgr24 -r %d/%d -an -y %s 2>&1";
        char buffer[256];
        snprintf(buffer, 256, format, stream.c_str(), metadata.width, metadata.height,
                 metadata.frameRateNum, 
                metadata.frameRateDenom, outputFormat.c_str());
        execAndGetOutput(std::string(buffer));
        exit(1);
    }

    return pid;
}

int FFMPEGProcessing::outputFramesToStreamFromPipe(const std::string& stream,
                                           const std::string& pipeName,
                                           const VideoMetadata& metadata)
{
    int pid = fork();
    if (pid < 0)
    {
        Config::output.log("Error forking to produce outgoing stream\n",
                           OutputWriter::LogLevel::Debug);
    }
    else if (pid == 0) // Child
    {
        const char* format = 
            "ffmpeg -f rawvideo -vcodec rawvideo -video_size %dx%d "
            "-pix_fmt bgr24 -framerate %d/%d -i %s -q:v 10 -r %d/%d "
            "-f rtsp %s 2>&1";
        char buffer[256];
        snprintf(buffer, 256, format, metadata.width, metadata.height,
                 metadata.frameRateNum, metadata.frameRateDenom, 
                 pipeName.c_str(),
                 metadata.frameRateNum, metadata.frameRateDenom,
                 stream.c_str());
        
        execAndGetOutput(std::string(buffer));
        exit(1);
    }

    return pid;
}

void FFMPEGProcessing::combineFrames(const std::string& inputFormat,
                                     const std::string& outputName,
                                     const VideoMetadata& metadata)
{
    // TODO: parameterize the input encoding
    const char* format = 
        "ffmpeg -y -r %d/%d -start_number 1 -f image2 -i "
        "%s -c:v libx264 %s 2>&1 &";
    char buffer[256];
    snprintf(buffer, 256, format, metadata.frameRateNum, 
            metadata.frameRateDenom, inputFormat.c_str(), outputName.c_str());
    execAndGetOutput(std::string(buffer));
}
