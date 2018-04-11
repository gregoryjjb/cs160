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

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

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

static cv::Rect GetBoundingRect(const cv::Mat_<double>& points)
{
    double minX = points.at<double>(0,0), 
            maxX = points.at<double>(0,0), 
            minY = points.at<double>(points.rows / 2,0), 
            maxY = points.at<double>(points.rows / 2,0);
    
    for (int i = 0; i < points.rows / 2; i++)
    {
        double x = points.at<double>(i, 0);
        double y = points.at<double>(i + points.rows / 2, 0);
        
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
    
    return cv::Rect(minX, minY, (maxX - minX), (maxY - minY));
}

static cv::Rect GetBoundingRect(const cv::Mat_<double>& points, int start, int end)
{
    double minX = points.at<double>(start,0), 
            maxX = points.at<double>(start,0), 
            minY = points.at<double>(start + (points.rows / 2),0), 
            maxY = points.at<double>(start + (points.rows / 2),0);
    
    for (int i = start; i < end; i++)
    {
        double x = points.at<double>(i, 0);
        double y = points.at<double>(i + points.rows / 2, 0);
        
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
    
    return cv::Rect(minX, minY, (maxX - minX), (maxY - minY));
}

/* Remove if already defined */
typedef long long int64;
typedef unsigned long long uint64;

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */

static uint64 GetTimeMs64()
{
#ifdef _WIN32
    /* Windows */
    FILETIME ft;
    LARGE_INTEGER li;

    /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
     * to a LARGE_INTEGER structure. */
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    uint64 ret = li.QuadPart;
    ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
    ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

    return ret;
#else
    /* Linux */
    struct timeval tv;

    gettimeofday(&tv, NULL);

    uint64 ret = tv.tv_usec;
    /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
    ret /= 1000;

    /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
    ret += (tv.tv_sec * 1000);

    return ret;
#endif
}

}

#endif /* UTILITIES_H */

