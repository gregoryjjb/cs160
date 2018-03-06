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
    
    void outputMetadata(const VideoMetadata& metadata);
    void outputFrameData(const FrameData& frameData);
    
private:
    std::mutex m_mutex;
};

#endif /* OUTPUTWRITER_H */

