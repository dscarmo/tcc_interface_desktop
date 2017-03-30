#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"


std::vector<cv::Rect> faceDetect(cv::Mat frame);
cv::Mat drawFps(cv::Mat input);
int testFaceDetection();