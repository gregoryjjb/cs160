/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StreamingFrameSource.h
 * Author: dean
 *
 * Created on March 17, 2018, 12:05 AM
 */

#ifndef STREAMINGFRAMESOURCE_H
#define STREAMINGFRAMESOURCE_H

#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>
#include <string>

#include "VideoMetadata.h"

// Abstracts the handling of a streaming frame sequence
class StreamingFrameSource
{
public:
    StreamingFrameSource(const VideoMetadata& metadata);
    
    ~StreamingFrameSource();
    
    void openFIFO(const std::string& inFifoName);
    
    // Gets the latest frame pulled from the stream source
    cv::Mat getLatestFrame();
    
private:
    bool m_Initialized;
    
    int m_InFifo;
    // Mutex to handle access to latest image variable
    std::mutex m_ImageLock;
    std::thread m_ImageFetchingThread;
    // The latest image pulled from the streaming input source
    cv::Mat m_LatestImage;
    unsigned int m_FrameWidth;
    unsigned int m_FrameHeight;
};

#endif /* STREAMINGFRAMESOURCE_H */

