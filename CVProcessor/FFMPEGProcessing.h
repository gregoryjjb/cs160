/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FFMPEGProcessing.h
 * Author: dean
 *
 * Created on February 23, 2018, 5:49 PM
 */

#ifndef FFMPEGPROCESSING_H
#define FFMPEGPROCESSING_H

#include <string>
#include "VideoMetadata.h"

namespace FFMPEGProcessing
{

/*
 * Extracts the video metadata from a given video file
 */
VideoMetadata extractMetadata(const std::string& videoFile);

/*
 * Extracts the inidividual frames from a given video file,
 * placing them in the given output folder
 */
void extractFrames(const std::string& videoFile, 
                   const std::string& outputFolder, 
                   const VideoMetadata& metadata);

}

#endif /* FFMPEGPROCESSING_H */

