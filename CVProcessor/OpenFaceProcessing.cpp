/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <tbb/tbb.h>

#include <string>
#include <vector>
#include <iostream>
#include <OpenFace/LandmarkDetectorParameters.h>
#include <OpenFace/ImageCapture.h>
#include <OpenFace/LandmarkDetectorModel.h>
#include <OpenFace/LandmarkDetectorFunc.h>
#include <OpenFace/Visualizer.h>
//#include <OpenFace/VisualizerUtils.h>

#include "Utilities.h"
#include "OpenFaceProcessing.h"
#include "VideoMetadata.h"

using namespace OpenFaceProcessing;

FaceDataPointsRecord::FaceDataPointsRecord(const cv::Mat_<double>& landmarks, 
                                           const cv::Mat_<int>& visibilities)
    : landmarks(landmarks),
    visibilities(visibilities)
{
    
}

FaceDataPointsRecord OpenFaceProcessing::extractFaceDataPoints(const std::string& imagePath, 
                                           const VideoMetadata& metadata)
{
    // TODO: figure out why these args matter
    std::vector<std::string> args{};
    args.push_back("-q");
    args.push_back("-wild");

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    cv::Mat_<uchar> grayImage;
    Utilities::ConvertToGrayscale_8bit(image, grayImage);
    
    LandmarkDetector::FaceModelParameters detParameters(args);
    std::cout << detParameters.model_location << std::endl;
    LandmarkDetector::CLNF clnfModel(detParameters.model_location);
    
    LandmarkDetector::DetectLandmarksInImage(grayImage, clnfModel, detParameters);
    
    return FaceDataPointsRecord(clnfModel.detected_landmarks, clnfModel.GetVisibilities());
}

void OpenFaceProcessing::applyFaceDataPointsToImage(const std::string& imagePath,
                                const std::string& outputPath,
                                const FaceDataPointsRecord& dataPoints,
                                const VideoMetadata& metadata)
{
    Utilities::Visualizer visualizer(false, false, false);
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    
    // estimates
    float fx = 500.0f * (metadata.width / 640.0f);
    float fy = 500.0f * (metadata.height / 480.0f);
    float cx = metadata.width / 2.0f;
    float cy = metadata.height / 2.0f;
    
    visualizer.SetImage(image, fx, fy, cx, cy);
    visualizer.SetObservationLandmarks(dataPoints.landmarks, 1.0, 
                                       dataPoints.visibilities);
    
    cv::Mat result = visualizer.GetVisImage();
    cv::imwrite(outputPath, result);
}