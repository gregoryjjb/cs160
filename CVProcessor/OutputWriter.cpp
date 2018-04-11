#include <sstream>
#include <iomanip>

#include "OutputWriter.h"
#include "json.hpp"

using nlohmann::json;

OutputWriter::OutputWriter()
    : m_mutex(),
    m_stdout(NULL),
    logLevel(LogLevel::Data)
{
}

void OutputWriter::enableOtherStdOutStreams()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    // Restore std::couts underlying stream
    std::cout.rdbuf(m_stdout);
}

void OutputWriter::disableOtherStdOutStreams()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    // Store std::couts underlying stream for our own use
    m_stdout = cout.rdbuf();
    // Disables std::couts underlying stream
    cout.rdbuf(NULL);
}

void OutputWriter::outputMetadata(const VideoMetadata& metadata)
{
    json j = {
        {"width", metadata.width},
        {"height", metadata.height},
        {"numFrames", metadata.numFrames},
        {"frameRateNum", metadata.frameRateNum},
        {"frameRateDenom", metadata.frameRateDenom}
    };

    log(j.dump(), OutputWriter::LogLevel::Data);
}

void OutputWriter::outputFrameData(const FrameData& frameData)
{
    std::ostringstream outputStream;
    outputStream.precision(5);
    outputStream.setf(std::ios::fixed);
    
    outputStream << std::endl;
    outputStream << "Begin Frame " << frameData.frameNumber << std::endl;
    
    outputStream << "Begin Landmarks" << std::endl;
    for (int i = 0; i < frameData.dataPoints.landmarks.rows / 2; i++)
    {
        double x = frameData.dataPoints.landmarks.at<double>(i, 0);
        double y = frameData.dataPoints.landmarks.at<double>(
            i + frameData.dataPoints.landmarks.rows / 2, 0);
        
        outputStream << i << ": " << x << " " << y << std::endl;
    }
    outputStream << "End Landmarks" << std::endl;
    
    outputStream << "Begin Head Pose" << std::endl;
    outputStream << "X: " << frameData.headPose[0] 
        << " Y: " << frameData.headPose[1]
        << " Z: " << frameData.headPose[2] 
        << std::endl;
    outputStream << "RX: " << frameData.headPose[3] 
        << " RY: " << frameData.headPose[4]
        << " RZ: " << frameData.headPose[5] 
        << std::endl;
    outputStream << "End Head Pose" << std::endl;
    
    outputStream << "Begin Pupils" << std::endl;
    cv::Point left, right;
    std::tie(left, right) = frameData.pupils;
    outputStream << left.x << " " << left.y << std::endl;
    outputStream << right.x << " " << right.y << std::endl;
    outputStream << "End Pupils" << std::endl;
    
    outputStream << "Begin Triangles" << std::endl;
    for (int i = 0; i < frameData.delaunayTriangles.size(); i++)
    {
        outputStream 
            << frameData.delaunayTriangles[i][0] << " "
            << frameData.delaunayTriangles[i][1] << " "
            << std::endl;
        
        outputStream 
            << frameData.delaunayTriangles[i][2] << " "
            << frameData.delaunayTriangles[i][3] << " "
            << std::endl;
        
        outputStream 
            << frameData.delaunayTriangles[i][4] << " "
            << frameData.delaunayTriangles[i][5] << " "
            << std::endl;
    }
    outputStream << "End Triangles" << std::endl;
    
    outputStream << "End Frame " << frameData.frameNumber << std::endl;
    outputStream << std::endl;
    
    log(outputStream.str(), OutputWriter::LogLevel::Data);
}

void OutputWriter::log(const std::string& str, OutputWriter::LogLevel level)
{
    if ((int)logLevel < (int)level)
        return;
    
    // Start critical section
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (m_stdout == NULL)
            std::cout << str;
        else
        {
            std::cout.rdbuf(m_stdout);
            std::cout << str;
            m_stdout = cout.rdbuf();
            std::cout.rdbuf(NULL);
        }
    }
}
