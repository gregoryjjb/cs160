/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VideoMetadata.h
 * Author: dean
 *
 * Created on February 23, 2018, 3:43 PM
 */

#ifndef VIDEOMETADATA_H
#define VIDEOMETADATA_H

struct VideoMetadata
{
public:
    int width, height, numFrames, frameRateNum, frameRateDenom;
    
    VideoMetadata(int w, int h, int frameCount, int frameRateNum, int frameRateDenom)
        : width(w),
        height(h),
        numFrames(frameCount),
        frameRateNum(frameRateNum),
        frameRateDenom(frameRateDenom)
    {}
    
private:
};

#endif /* VIDEOMETADATA_H */

