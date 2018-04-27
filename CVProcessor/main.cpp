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

#include <boost/filesystem.hpp>

#include "Config.h"
#include "Utilities.h"
#include "VideoProcessing.h"

OutputWriter Config::output;
std::string Config::outputPrefix;
std::string Config::targetFile;
std::string Config::videoStream;
std::string Config::outputVideoName;
std::string Config::outputFormat = "-f webm -c:v vp8 -b:v .5M";
Config::ExecutionMode Config::execMode;
std::string Config::cmdFrameRate;
int Config::cmdWidth;
int Config::cmdHeight;

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

// Configure application based on the given command line args
void initializeConfiguration(const std::vector<std::string>& args)
{
    for (int i = 0; i < args.size(); i++)
    {
        // Fixed file processing
        if (args[i] == "-f")
        {
            Config::execMode = Config::ExecutionMode::VideoFile;
            Config::targetFile = args[i+1];
            Config::outputVideoName = "processed.mp4";
            i++;
        }
        else if (args[i] == "-s")
        {
            Config::execMode = Config::ExecutionMode::VideoStream;
            Config::videoStream = args[i+1];
            i++;

            // In VideoStream mode, we can't output anything to stdout
            // except frame data
            Config::output.setEnabled(false);
        }
        else if (args[i] == "-stdio")
        {
            Config::execMode = Config::ExecutionMode::StandardIO;
            Config::videoStream = "pipe:0";
            Config::cmdFrameRate = args[i+1];
            Config::cmdWidth = std::stoi(args[i+2]);
            Config::cmdHeight = std::stoi(args[i+3]);
            i += 3;
            
            // In StandardIO mode, we can't output anything to stdout
            // except frame data
            Config::output.setEnabled(false);
        }
        // Output name
        else if (args[i] == "-o")
        {
            Config::outputVideoName = args[i+1];
            i++;
        }
        else if (args[i] == "-of")
        {
            Config::outputFormat = args[i+1];
            i++;
        }
        // Log Level
        else if (args[i] == "-l")
        {
            Config::output.logLevel = (OutputWriter::LogLevel)std::stoi(args[i+1]);
            i++;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc <= 1)
        return 0;
    
    Config::outputPrefix = Utilities::GenerateRandomString(10);
    
    // Create output directories
    boost::filesystem::create_directory(Config::outputPrefix + "/");   

    initializeConfiguration(
        parseArguments(argc, argv));
    
    Utilities::uint64 tStart = Utilities::GetTimeMs64();
    
    if (Config::execMode == Config::ExecutionMode::VideoFile)
    {
        // Disable std::out to prevent libraries
        // like OpenFace and OpenCV from spamming
        // our output
        Config::output.disableOtherStdOutStreams();
        VideoProcessing::processVideo(Config::targetFile, Config::outputVideoName);
    }
    else if (Config::execMode == Config::ExecutionMode::StandardIO
            || Config::execMode == Config::ExecutionMode::VideoStream)
    {
        VideoProcessing::processVideoStream(Config::videoStream);
    }

    Utilities::uint64 tEnd = Utilities::GetTimeMs64();

    Config::output.log("Full Processing Took: " + std::to_string(tEnd - tStart) + "ms\n",
                       OutputWriter::LogLevel::Info);

    boost::filesystem::remove_all(Config::outputPrefix + "/");
    
    return 0;
}
