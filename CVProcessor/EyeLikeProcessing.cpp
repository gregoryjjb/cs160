/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "EyeLike/findEyes.h"

#include "EyeLike/constants.h"
#include "EyeLikeProcessing.h"

const std::string FaceCascadeLocation = "haarcascade_frontalface_alt.xml";

float kEyeCornerKernel[4][6] = {
  {-1,-1,-1, 1, 1, 1},
  {-1,-1,-1,-1, 1, 1},
  {-1,-1,-1,-1, 0, 3},
  { 1, 1, 1, 1, 1, 1},
};

std::tuple<cv::Point, cv::Point> EyeLikeProcessing::detectPupils(const cv::Mat_<uchar>& grayFrame)
{
    cv::CascadeClassifier faceCascade;

    if (!faceCascade.load(FaceCascadeLocation))
    {
        std::cout << "Unable to load facial cascade - check path" << std::endl;
    }

    // Create eye kernels
    cv::Mat rightCornerKernel(4, 6, CV_32F, kEyeCornerKernel);
    cv::Mat leftCornerKernel(4, 5, CV_32F);
    cv::flip(rightCornerKernel, leftCornerKernel, 1);

    std::vector<cv::Rect> faces;

    faceCascade.detectMultiScale(grayFrame, faces, 1.1, 2,
                                 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));

    if (faces.size() > 0)
    {
        return findEyes(grayFrame, faces[0]);
    }
    
    return std::make_tuple(cv::Point(0,0), cv::Point(0,0));
}

void EyeLikeProcessing::applyEyeCentersToImage(cv::Mat& outputImage, 
                                               const std::tuple<cv::Point,cv::Point>& pupilLocations)
{
    cv::Point left;
    cv::Point right;
    std::tie(left, right) = pupilLocations;
    cv::circle(outputImage, left, 3, 200);
    cv::circle(outputImage, right, 3, 200);
}