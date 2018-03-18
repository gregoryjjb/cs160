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
std::string Config::targetFile;
std::string Config::videoStream;
std::string Config::outputVideoName;
Config::ExecutionMode Config::execMode;

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
        // Stream processing
        else if (args[i] == "-s")
        {
            Config::execMode = Config::ExecutionMode::VideoStream;
            Config::videoStream = args[i+1];
            Config::outputVideoName = "rtsp://localhost:5545/processed.mp4";
            i++;
        }
        // Output name
        else if (args[i] == "-o")
        {
            Config::outputVideoName = args[i+1];
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
    
    // Delete existing output directories to clear old data
    boost::filesystem::remove_all("frames/");
    boost::filesystem::remove_all("processed/");
    
    // Create output directories
    boost::filesystem::create_directory("frames/");
    boost::filesystem::create_directory("processed/");

    initializeConfiguration(
        parseArguments(argc, argv));
    
    Utilities::uint64 tStart = Utilities::GetTimeMs64();
    // Disable std::out to prevent libraries
    // like OpenFace and OpenCV from spamming
    // our output
    Config::output.disableOtherStdOutStreams();
    
    if (Config::execMode == Config::ExecutionMode::VideoFile)
    {
        VideoProcessing::processVideo(Config::targetFile, Config::outputVideoName);
    }
    else if (Config::execMode == Config::ExecutionMode::VideoStream)
    {
        VideoProcessing::processVideoStream(Config::videoStream, Config::outputVideoName);
    }

    Utilities::uint64 tEnd = Utilities::GetTimeMs64();

    Config::output.log("Full Processing Took: " + std::to_string(tEnd - tStart) + "ms\n",
                       OutputWriter::LogLevel::Info);

    return 0;
}
