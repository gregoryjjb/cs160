/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <memory>
#include <array>

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include "SystemHelper.h"

#include "Config.h"

std::string execAndGetOutput(const std::string& cmd)
{
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

int createFIFO(const std::string& fifoName)
{
    int pipeStatus = mkfifo(fifoName.c_str(), 0622);
    if (pipeStatus < 0)
    {
        Config::output.log("Failed to create " + fifoName + " pipe(already created?)\n",
                           OutputWriter::LogLevel::Debug);
    }
    
    return pipeStatus;
}