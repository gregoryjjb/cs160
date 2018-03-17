/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EyeLikeProcessing.h
 * Author: dean
 *
 * Created on March 3, 2018, 7:03 PM
 */

#ifndef EYELIKEPROCESSING_H
#define EYELIKEPROCESSING_H

#include <string>
#include <opencv2/core/core.hpp>
#include <tuple>

namespace EyeLikeProcessing
{

// Returns the location of the pupils in the first detected face
// of the given image
std::tuple<cv::Point, cv::Point> detectPupils(const cv::Mat_<uchar>& grayFrame);

// Draws the given pupil locations onto the given image
void applyEyeCentersToImage(cv::Mat& outputImage, 
                            const std::tuple<cv::Point, cv::Point>& pupilLocations);
}

#endif /* EYELIKEPROCESSING_H */

