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
#include <functional>
#include "VideoMetadata.h"

namespace FFMPEGProcessing
{

/*
 * Extracts the video metadata from a given video file
 */
VideoMetadata extractMetadata(const std::string& videoFile);

/*
 * Extracts the video metadata from a given video stream
 */
VideoMetadata extractMetadataFromStream(const std::string& stream);

/*
 * Extracts the inidividual frames from a given video file,
 * placing them in the given output folder
 */
void extractFrames(const std::string& videoFile, 
                   const std::string& outputFormat, 
                   const VideoMetadata& metadata);

/*
 * Begins extracting frames from the given stream.
 * Returns the PID of the process managing the stream extraction.
 */
int extractFramesFromStream(const std::string& stream,
                             const std::string& outputFormat,
                             const VideoMetadata& metadata);

/*
 * Begins outputting files from the given pipe to the given stream.
 * Returns the PID of the process managing the stream output.
 */
int outputFramesToStreamFromPipe(const std::string& stream,
                          const std::string& pipeName,
                          const VideoMetadata& metadata);
/*
 * Combines the frames of a given format on disk
 * into a video file of the given name
 */
void combineFrames(const std::string& inputFormat,
                   const std::string& outputName,
                   const VideoMetadata& metadata);

}

#endif /* FFMPEGPROCESSING_H */

