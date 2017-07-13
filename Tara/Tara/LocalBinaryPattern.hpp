#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "TDLBP.h"
#include "Utils.hpp"
//#include <windef.h>

#define FEATURE_SIZE 256 * 16 * 16

class LocalBinaryPattern {
private:
	//LBP perfomance timer
	Timer timer;

	//Calcula LBP clássico
	cv::Mat LocalBinaryPattern::grayImageLBP(cv::Mat grayImage);

	//LBP from net
	cv::Mat LocalBinaryPattern::LBP(cv::Mat img);

	//Recebe imagem do LBP clássico pra gerar um histograma
	double** LocalBinaryPattern::getHistogram(cv::Mat lbpImage);
public:
	//Testa com imagem da webcam
	void LocalBinaryPattern::webCamTest();

	cv::Mat LocalBinaryPattern::LBPHistogram(cv::Mat lbp);
	cv::Mat LocalBinaryPattern::grayLBPpipeline(cv::Mat frame);

	cv::Mat LocalBinaryPattern::betterLBPpipeline(cv::Mat frame);
	LocalBinaryPattern() {
		timer = Timer();
	}

};
