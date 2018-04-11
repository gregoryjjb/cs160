#include "findEyes.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"
#include "constants.h"

std::tuple<cv::Point, cv::Point> findEyes(const cv::Mat& frameGray, const cv::Rect& face)
{
    cv::Mat faceROI = frameGray(face);

    if (kSmoothFaceImage)
    {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
    }
    //-- Find eye regions and draw them
    int eye_region_width = face.width * (kEyePercentWidth / 100.0);
    int eye_region_height = face.width * (kEyePercentHeight / 100.0);
    int eye_region_top = face.height * (kEyePercentTop / 100.0);
    cv::Rect leftEyeRegion(face.width * (kEyePercentSide / 100.0),
                           eye_region_top, eye_region_width, eye_region_height);
    cv::Rect rightEyeRegion(face.width - eye_region_width - face.width * (kEyePercentSide / 100.0),
                            eye_region_top, eye_region_width, eye_region_height);
    
    //-- Find Eye Centers
    cv::Point leftPupil = findEyeCenter(faceROI, leftEyeRegion);
    cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion);

    // change eye centers to face coordinates
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    
    rightPupil.x += face.x;
    rightPupil.y += face.y;
    leftPupil.x += face.x;
    leftPupil.y += face.y;
    
    return std::make_tuple(leftPupil, rightPupil);
}

std::tuple<cv::Point, cv::Point> findEyes(const cv::Mat& frameGray,
                                          const cv::Rect& face,
                                          const cv::Rect& leftEyeRegion,
                                          const cv::Rect& rightEyeRegion)
{
    cv::Mat faceROI = frameGray(face);

    if (kSmoothFaceImage)
    {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
    }
    
    //-- Find Eye Centers
    cv::Point leftPupil = findEyeCenter(faceROI, leftEyeRegion);
    cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion);
    
    // change eye centers to face coordinates
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    
    rightPupil.x += face.x;
    rightPupil.y += face.y;
    leftPupil.x += face.x;
    leftPupil.y += face.y;
    
    return std::make_tuple(leftPupil, rightPupil);
}