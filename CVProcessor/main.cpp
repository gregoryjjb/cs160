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

void initializeConfiguration(const std::vector<std::string>& args)
{
    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] == "-f")
        {
            Config::execMode = Config::ExecutionMode::VideoFile;
            Config::targetFile = args[i+1];
            i++;
        }
        else if (args[i] == "-s")
        {
            Config::execMode = Config::ExecutionMode::VideoStream;
            Config::videoStream = args[i+1];
            i++;
        }
        else if (args[i] == "-o")
        {
            Config::outputVideoName = args[i+1];
            i++;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc <= 1)
        return 0;
    
    boost::filesystem::remove_all("frames/");
    boost::filesystem::remove_all("processed/");
    
    boost::filesystem::create_directory("frames/");
    boost::filesystem::create_directory("processed/");
    
    Config::outputVideoName = "processed.mp4";
    
    initializeConfiguration(
        parseArguments(argc, argv));
    
    Utilities::uint64 tStart = Utilities::GetTimeMs64();
    Config::output.disableOtherStdOutStreams();
    
    if (Config::execMode == Config::ExecutionMode::VideoFile)
    {
        VideoProcessing::processVideo(Config::targetFile, Config::outputVideoName);
    }
    else if (Config::execMode == Config::ExecutionMode::VideoStream)
    {
        VideoProcessing::processVideoStream(Config::videoStream);
    }

    Utilities::uint64 tEnd = Utilities::GetTimeMs64();

    std::cout << "Full Processing Took: " << (tEnd - tStart) << "ms" << std::endl;

    return 0;
}
