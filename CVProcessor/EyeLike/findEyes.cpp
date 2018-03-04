#include "findEyes.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"
#include "constants.h"

std::tuple<cv::Point, cv::Point> findEyes(const cv::Mat& frameGray, const cv::Rect& face)
{
    cv::Mat faceROI = frameGray(face);
    //cv::Mat debugFace = faceROI;

    /*if (kSmoothFaceImage)
    {
        double sigma = kSmoothFaceFactor * face.width;
        GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
    }*/
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
    // get corner regions
    /*cv::Rect leftRightCornerRegion(leftEyeRegion);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    cv::Rect leftLeftCornerRegion(leftEyeRegion);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    cv::Rect rightLeftCornerRegion(rightEyeRegion);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    cv::Rect rightRightCornerRegion(rightEyeRegion);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
    rectangle(debugFace, leftRightCornerRegion, 200);
    rectangle(debugFace, leftLeftCornerRegion, 200);
    rectangle(debugFace, rightLeftCornerRegion, 200);
    rectangle(debugFace, rightRightCornerRegion, 200);*/
    // change eye centers to face coordinates
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    
    rightPupil.x += face.x;
    rightPupil.y += face.y;
    leftPupil.x += face.x;
    leftPupil.y += face.y;
    // draw eye centers
    /*circle(debugFace, rightPupil, 3, 1234);
    circle(debugFace, leftPupil, 3, 1234);/*

    //-- Find Eye Corners
    /*if (kEnableEyeCorner)
    {
        cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
        leftRightCorner.x += leftRightCornerRegion.x;
        leftRightCorner.y += leftRightCornerRegion.y;
        cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
        leftLeftCorner.x += leftLeftCornerRegion.x;
        leftLeftCorner.y += leftLeftCornerRegion.y;
        cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
        rightLeftCorner.x += rightLeftCornerRegion.x;
        rightLeftCorner.y += rightLeftCornerRegion.y;
        cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
        rightRightCorner.x += rightRightCornerRegion.x;
        rightRightCorner.y += rightRightCornerRegion.y;
        circle(faceROI, leftRightCorner, 3, 200);
        circle(faceROI, leftLeftCorner, 3, 200);
        circle(faceROI, rightLeftCorner, 3, 200);
        circle(faceROI, rightRightCorner, 3, 200);
    }

    //imshow(face_window_name, faceROI);*/
    
    return std::make_tuple(leftPupil, rightPupil);
}