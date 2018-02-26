/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utilities.h
 * Author: dean
 *
 * Created on February 24, 2018, 2:52 AM
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

namespace Utilities
{

//===========================================================================
// Converting between color spaces and bit depths
//===========================================================================

static void ConvertToGrayscale_8bit(const cv::Mat& in, cv::Mat& out)
{
    if (in.channels() == 3)
    {
        // Make sure it's in a correct format
        if (in.depth() == CV_16U)
        {
            cv::Mat tmp = in / 256;
            tmp.convertTo(out, CV_8U);
            cv::cvtColor(out, out, CV_BGR2GRAY);
        }
        else
        {
            cv::cvtColor(in, out, CV_BGR2GRAY);
        }
    }
    else if (in.channels() == 4)
    {
        if (in.depth() == CV_16U)
        {
            cv::Mat tmp = in / 256;
            tmp.convertTo(out, CV_8U);
            cv::cvtColor(out, out, CV_BGRA2GRAY);
        }
        else
        {
            cv::cvtColor(in, out, CV_BGRA2GRAY);
        }
    }
    else
    {
        if (in.depth() == CV_16U)
        {
            cv::Mat tmp = in / 256;
            tmp.convertTo(out, CV_8U);
        }
        else if (in.depth() == CV_8U)
        {
            out = in.clone();
        }
    }
}


}

#endif /* UTILITIES_H */

