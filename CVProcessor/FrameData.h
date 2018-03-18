#ifndef FRAMEDATA_H
#define FRAMEDATA_H

#include "OpenFaceProcessing.h"

// Encapsulates the processed data associated with
// a single frame
struct FrameData
{    
    int frameNumber;
    OpenFaceProcessing::FaceDataPointsRecord dataPoints;
    cv::Vec6f headPose;
    std::vector<cv::Vec6f> delaunayTriangles;
    cv::Mat outputImage;
    std::tuple<cv::Point, cv::Point> pupils;
    
    FrameData()
        : frameNumber(-1),
        dataPoints(),
        headPose(),
        delaunayTriangles(),
        outputImage(),
        pupils()
    {
    }
};

#endif /* FRAMEDATA_H */

