/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VideoProcessing.h
 * Author: dean
 *
 * Created on March 8, 2018, 5:49 PM
 */

#ifndef VIDEOPROCESSING_H
#define VIDEOPROCESSING_H

#include <string>

namespace VideoProcessing
{

void processVideo(const std::string& inputFile, 
                  const std::string& outputFile);

void processVideoStream();

}

#endif /* VIDEOPROCESSING_H */

