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
#include <string>
#include <array>

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

void splitFrames(const std::string& targetVideo, const std::string& outputFolder, int fps)
{
    // TODO: Validate that we wont overflow buffer
    std::string format = "ffmpeg -i %s -vf fps=%d %s/out%d.png 2>&1";
    char buffer[256];
    sprintf(buffer, format.c_str(), targetVideo.c_str(), fps, outputFolder.c_str());
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
        splitFrames(inputFile, "frames", 30);
    }
    return 0;
}

