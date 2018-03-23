/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StreamingFrameSource.h"
#include "Config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

StreamingFrameSource::StreamingFrameSource(const VideoMetadata& metadata)
    : m_FrameWidth(metadata.width),
    m_FrameHeight(metadata.height),
    m_LatestImage(metadata.width, metadata.height, CV_8UC3),
    m_ImageLock(),
    m_InFifo(0),
    m_Initialized(false)
{
    
}

StreamingFrameSource::~StreamingFrameSource()
{
    close(m_InFifo);
}

void StreamingFrameSource::openFIFO(const std::string& inFifoName)
{
    if ((m_InFifo = open(inFifoName.c_str(), O_RDONLY)) < 0)
    {
        Config::output.log("Unable to open input pipe\n",
                           OutputWriter::LogLevel::Debug);
        return;
    }
    
    m_Initialized = true;
    
    m_ImageFetchingThread = std::thread([&]()
    {
        while (true)
        {
            uchar buffer[m_FrameWidth*m_FrameHeight*3];
        
            unsigned int current = 0; // current point in buffer
            unsigned int limit = m_FrameWidth*m_FrameHeight*3; // last byte

            // Read the full length of the buffer
            while (current < limit)
            {
                unsigned int bytesRead = read(m_InFifo, buffer + current, limit - current);
                current += bytesRead;
            }

            cv::Mat image(m_FrameHeight, m_FrameWidth, CV_8UC3, buffer);
            if (image.empty()) {
                Config::output.log("Failed to read image\n",
                                   OutputWriter::LogLevel::Debug);
            }
            
            {
                std::unique_lock<std::mutex> lock(m_ImageLock);
                m_LatestImage = image.clone();
            }
        }
    });
}

cv::Mat StreamingFrameSource::getLatestFrame()
{
    if (!m_Initialized)
    {
        Config::output.log("Cannot call getLatestFrame on uninitialized StreamingFrameSource\n",
                           OutputWriter::LogLevel::Debug);
        return cv::Mat();
    }
    
    std::unique_lock<std::mutex> lock(m_ImageLock);
    return m_LatestImage.clone();
}