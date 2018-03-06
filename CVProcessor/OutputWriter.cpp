#include <sstream>
#include <iomanip>

#include "OutputWriter.h"

OutputWriter::OutputWriter()
    : m_mutex()
{
}

void OutputWriter::outputMetadata(const VideoMetadata& metadata)
{
    std::ostringstream outputStream;
    
    outputStream << "Begin Metadata" << std::endl;
    
    outputStream << "Width: " << metadata.width << std::endl;
    outputStream << "Height: " << metadata.height << std::endl;
    outputStream << "NumFrames: " << metadata.numFrames << std::endl;
    outputStream << "FrameRateNum: " << metadata.frameRateNum << std::endl;
    outputStream << "FrameRateDenom: " << metadata.frameRateDenom << std::endl;
    
    outputStream << "End Metadata" << std::endl;
    
    // Start critical section
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cout << outputStream.str();
    }
}

void OutputWriter::outputDataPoints(int frameNumber,
    const OpenFaceProcessing::FaceDataPointsRecord& dataPoints)
{
    std::ostringstream outputStream;
    outputStream.precision(5);
    outputStream.setf(std::ios::fixed);
    
    outputStream << "Begin Frame " << frameNumber << "Landmarks" << std::endl;
    
    for (int i = 0; i < dataPoints.landmarks.rows / 2; i++)
    {
        double x = dataPoints.landmarks.at<double>(i, 0);
        double y = dataPoints.landmarks.at<double>(i + dataPoints.landmarks.rows / 2, 0);
        
        outputStream << i << ": " << x << " " << y << std::endl;
    }
    
    outputStream << "End Frame " << frameNumber << "Landmarks" << std::endl;
    
    // Start critical section
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cout << outputStream.str();
    }
}