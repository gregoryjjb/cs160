/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Config.h
 * Author: dean
 *
 * Created on March 6, 2018, 1:15 PM
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#include "OutputWriter.h"

namespace Config
{

OutputWriter output;

std::string targetFile;
std::string videoStream;
std::string outputVideoName = "processed.mp4";

enum class ExecutionMode
{
    VideoFile,
    VideoStream
} execMode;

}

#endif /* CONFIG_H */

