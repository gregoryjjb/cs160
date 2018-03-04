#ifndef FINDEYE_H
#define FINDEYE_H

#include <opencv2/core/core.hpp>
#include <tuple>

std::tuple<cv::Point, cv::Point> findEyes(const cv::Mat& frame_gray, const cv::Rect& face);

#endif /* FINDEYE_H */

