#pragma once

#include <opencv2/opencv.hpp>

void CV1();

cv::Vec3f GetSafeValue(cv::Mat* image, int x, int y);
void MultiplyByMatrix(cv::Mat* image, cv::Mat* result, cv::Matx33f matrix);

cv::Mat ToYUV(cv::Mat* image);
cv::Mat ToRGB(cv::Mat* image);