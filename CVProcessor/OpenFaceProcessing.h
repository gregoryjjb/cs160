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

#include "VideoMetadata.h"

namespace OpenFaceProcessing
{

void extractFaceDataPoints(const std::string& imagePath, 
                           const VideoMetadata& metadata);

}

#endif /* OPENFACEPROCESSING_H */

