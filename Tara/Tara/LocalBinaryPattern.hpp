#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include <windef.h>

class LocalBinaryPattern {
private:
	//Returns LBP binary word (eg. 00101101) for each 8 neighbourhood
	uchar roiLBP(cv::Mat input);
	
	cv::Mat LocalBinaryPattern::grayImageLBP(cv::Mat grayImage);

	void LocalBinaryPattern::webCamTest();

	//Recebe imagem do LBP pra gerar um histograma
	double** LocalBinaryPattern::getHistogram(cv::Mat lbpImage);
public: 
	int LocalBinaryPattern::test();
	void LocalBinaryPattern::grayLBPpipeline(cv::Mat frame);

};
