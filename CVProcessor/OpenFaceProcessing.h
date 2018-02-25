/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OpenFaceProcessing.h
 * Author: dean
 *
 * Created on February 23, 2018, 11:43 PM
 */

#ifndef OPENFACEPROCESSING_H
#define OPENFACEPROCESSING_H

#include <string>
#include <opencv2/core/core.hpp>
#include <OpenFace/LandmarkDetectorModel.h>

#include "VideoMetadata.h"

namespace OpenFaceProcessing
{

struct FaceDataPointsRecord
{
public:
    cv::Mat_<double> landmarks;
    cv::Mat_<int> visibilities;
    
    FaceDataPointsRecord(const cv::Mat_<double>& landmarks, 
                         const cv::Mat_<int>& visibilities);
};

FaceDataPointsRecord extractFaceDataPoints(const std::string& imagePath,
                                           const VideoMetadata& metadata);

FaceDataPointsRecord extractFaceDataPoints(const std::string& imagePath,
                                           const VideoMetadata& metadata,
                                           const LandmarkDetector::CLNF& originalModel);

void applyFaceDataPointsToImage(const std::string& imagePath,
                                const std::string& outputPath,
                                const FaceDataPointsRecord& dataPoints,
                                const VideoMetadata& metadata);

}

#endif /* OPENFACEPROCESSING_H */

