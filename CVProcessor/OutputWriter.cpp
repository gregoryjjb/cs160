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

    log(j.dump(2) + "\n", OutputWriter::LogLevel::Data);
}

void OutputWriter::outputFrameData(const FrameData& frameData)
{
    cv::Point leftPupil, rightPupil;
    std::tie(leftPupil, rightPupil) = frameData.pupils;
    
    json j = {
        {"frameNum", frameData.frameNumber},
        {"landmarks", "[]"_json},
        {"headPose", {
            {"x", frameData.headPose[0]},
            {"y", frameData.headPose[1]},
            {"z", frameData.headPose[2]},
            {"rx", frameData.headPose[3]},
            {"ry", frameData.headPose[4]},
            {"rz", frameData.headPose[5]}}
        },
        {"pupils",{
            {"lx", leftPupil.x},
            {"ly", leftPupil.y},
            {"rx", rightPupil.x},
            {"ry", rightPupil.y}}
        },
        {"triangles", "[]"_json}
    };
    
    for (int i = 0; i < frameData.dataPoints.landmarks.rows / 2; i++)
    {
        double x = frameData.dataPoints.landmarks.at<double>(i, 0);
        double y = frameData.dataPoints.landmarks.at<double>(
            i + frameData.dataPoints.landmarks.rows / 2, 0);
        
        j["landmarks"].push_back({{"x", x}, {"y", y}});
    }
    
    for (int i = 0; i < frameData.delaunayTriangles.size(); i++)
    {
        j["triangles"].push_back(json::array({
            frameData.delaunayTriangles[i][0],
            frameData.delaunayTriangles[i][1],
            frameData.delaunayTriangles[i][2],
            frameData.delaunayTriangles[i][3],
            frameData.delaunayTriangles[i][4],
            frameData.delaunayTriangles[i][5],
        }));
    }

    log(j.dump(2) + "\n", OutputWriter::LogLevel::Data);
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
