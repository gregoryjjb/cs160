#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <mutex>

#include "VideoMetadata.h"
#include "OpenFaceProcessing.h"

// Provides thread-safe output features
// for processed data
class OutputWriter
{
public:
    OutputWriter();
    
    void outputMetadata(const VideoMetadata& metadata);
    
    void outputDataPoints(int frameNumber, 
        const OpenFaceProcessing::FaceDataPointsRecord& dataPoints);
private:
    std::mutex m_mutex;
};

#endif /* OUTPUTWRITER_H */

