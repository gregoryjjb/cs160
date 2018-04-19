#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <mutex>

#include "VideoMetadata.h"
#include "FrameData.h"

// Provides thread-safe output features
// for processed data
class OutputWriter
{
public:
    OutputWriter();
    
    enum class LogLevel {
        Data, Info, Debug
    } logLevel;
    
    void setEnabled(bool value);
    void disableOtherStdOutStreams();
    void enableOtherStdOutStreams();
    void outputMetadata(const VideoMetadata& metadata);
    void outputFrameData(const FrameData& frameData);
    void log(const std::string& str, OutputWriter::LogLevel level);
    
private:
    bool m_enabled;
    std::mutex m_mutex;
    std::streambuf*  m_stdout;
};

#endif /* OUTPUTWRITER_H */

