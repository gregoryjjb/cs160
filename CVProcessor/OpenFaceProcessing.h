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

cv::Mat openImage(const std::string& inputPath);
void saveImage(const std::string& path, const cv::Mat& image);

// Extracts the facial data points from a single image
FaceDataPointsRecord extractFaceDataPoints(const cv::Mat_<uchar> grayImage,
                                           const VideoMetadata& metadata);

// Extracts the facial data points from a frame of a video
// using the provided CLNF model that is tracking each frame
FaceDataPointsRecord extractFaceDataPoints(const cv::Mat_<uchar> grayFrame,
                                           const VideoMetadata& metadata,
                                           LandmarkDetector::CLNF& model);

std::vector<cv::Vec6f> getDelaunayTriangles(const FaceDataPointsRecord& dataPoints,
                          const VideoMetadata& metadata);

void applyFaceDataPointsToImage(const std::string& imagePath,
                                const std::string& outputPath,
                                const FaceDataPointsRecord& dataPoints,
                                const VideoMetadata& metadata);

void applyFaceDataPointsToImage(const cv::Mat& inputImage,
                                cv::Mat& outputImage,
                                const FaceDataPointsRecord& dataPoints,
                                const VideoMetadata& metadata);

void applyDelaunayTrianlgesToImage(cv::Mat& outputImage,
                                   const std::vector<cv::Vec6f> triangles,
                                   const VideoMetadata& metadata);

}

#endif /* OPENFACEPROCESSING_H */

