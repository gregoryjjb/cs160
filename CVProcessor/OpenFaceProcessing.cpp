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

std::vector<std::string> detectionArgs{"-q"};

FaceDataPointsRecord::FaceDataPointsRecord()
    : landmarks(), visibilities()
{
}

FaceDataPointsRecord::FaceDataPointsRecord(const cv::Mat_<double>& landmarks,
                                           const cv::Mat_<int>& visibilities)
: landmarks(landmarks),
visibilities(visibilities)
{

}

cv::Mat OpenFaceProcessing::openImage(const std::string& inputPath)
{
    return cv::imread(inputPath, cv::IMREAD_COLOR);
}

void OpenFaceProcessing::saveImage(const std::string& path, const cv::Mat& image)
{
    cv::imwrite(path, image);
}

FaceDataPointsRecord OpenFaceProcessing::extractFaceDataPoints(const cv::Mat_<uchar> grayImage,
                                                               const VideoMetadata& metadata)
{
    LandmarkDetector::FaceModelParameters detParameters(detectionArgs);
    LandmarkDetector::CLNF clnfModel(detParameters.model_location);

    LandmarkDetector::DetectLandmarksInImage(grayImage, clnfModel, detParameters);

    return FaceDataPointsRecord(clnfModel.detected_landmarks, clnfModel.GetVisibilities());
}

FaceDataPointsRecord OpenFaceProcessing::extractFaceDataPoints(const cv::Mat_<uchar> grayFrame,
                                                               const VideoMetadata& metadata,
                                                               LandmarkDetector::CLNF& clnfModel)
{
    LandmarkDetector::FaceModelParameters detParameters(detectionArgs);

    LandmarkDetector::DetectLandmarksInVideo(grayFrame, clnfModel, detParameters);

    return FaceDataPointsRecord(clnfModel.detected_landmarks, clnfModel.GetVisibilities());
}

cv::Vec6f OpenFaceProcessing::extractHeadPose(const LandmarkDetector::CLNF& clnfModel, 
                          const VideoMetadata& metadata)
{
    return LandmarkDetector::GetPose(clnfModel, 500, 500, metadata.width / 2, metadata.height / 2);
}

std::vector<cv::Vec6f> OpenFaceProcessing::getDelaunayTriangles(const FaceDataPointsRecord& dataPoints,
                                                                const VideoMetadata& metadata)
{
    cv::Rect rect(0, 0, metadata.width, metadata.height);
    cv::Subdiv2D sub(rect);

    // TODO: use array to avoid resizing
    std::vector<cv::Point2f> points;
    for (int i = 0; i < dataPoints.landmarks.rows / 2; i++)
    {
        double x = dataPoints.landmarks.at<double>(i, 0);
        double y = dataPoints.landmarks.at<double>(i + dataPoints.landmarks.rows / 2, 0);
        points.push_back(cv::Point2f(x,y));
    }

    if (points.size() == 0)
        return std::vector<cv::Vec6f>();
    
    sub.insert(points);

    std::vector<cv::Vec6f> triangles;
    sub.getTriangleList(triangles);
    
    // Remove triangles outside the bounds of the image
    std::vector<cv::Point> pt(3);
    for (int i = triangles.size() - 1; i >= 0; i--)
    {
        cv::Vec6f t = triangles[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

        if (!rect.contains(pt[0]) || !rect.contains(pt[1]) || !rect.contains(pt[2]))
        {
            triangles.erase(triangles.begin() + i);
        }
    }
    
    return triangles;
}

void OpenFaceProcessing::applyFaceDataPointsToImage(cv::Mat& outputImage,
                                                    const FaceDataPointsRecord& dataPoints,
                                                    const VideoMetadata& metadata,
                                                    double scaleFactor)
{
    const cv::Scalar DATA_POINT_COLOR(200, 0, 0);
    
    for (int i = 0; i < dataPoints.landmarks.rows / 2; i++)
    {
        double x = dataPoints.landmarks.at<double>(i, 0);
        double y = dataPoints.landmarks.at<double>(i + dataPoints.landmarks.rows / 2, 0);
        cv::circle(outputImage, cv::Point(x * scaleFactor,y * scaleFactor), 3, DATA_POINT_COLOR);
    }
}

void OpenFaceProcessing::applyDelaunayTrianlgesToImage(cv::Mat& outputImage,
                                                       const std::vector<cv::Vec6f> triangles,
                                                       const VideoMetadata& metadata,
                                                       double scaleFactor)
{
    std::vector<cv::Point> pt(3);
    const cv::Scalar DELAUNAY_COLOR(255, 255, 255);
    cv::Rect rect(0, 0, metadata.width, metadata.height);

    for (int i = 0; i < triangles.size(); i++)
    {
        cv::Vec6f t = triangles[i];
        pt[0] = cv::Point(cvRound(t[0] * scaleFactor), cvRound(t[1] * scaleFactor));
        pt[1] = cv::Point(cvRound(t[2] * scaleFactor), cvRound(t[3] * scaleFactor));
        pt[2] = cv::Point(cvRound(t[4] * scaleFactor), cvRound(t[5] * scaleFactor));

        if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
        {
            cv::line(outputImage, pt[0], pt[1], DELAUNAY_COLOR, 1, CV_AA, 0);
            cv::line(outputImage, pt[1], pt[2], DELAUNAY_COLOR, 1, CV_AA, 0);
            cv::line(outputImage, pt[2], pt[0], DELAUNAY_COLOR, 1, CV_AA, 0);
        }
    }
}