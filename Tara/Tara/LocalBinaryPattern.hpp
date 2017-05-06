#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "TDLBP.h"
//#include <windef.h>

class LocalBinaryPattern {
private:
	//DEPRECATED Returns LBP binary word (eg. 00101101) for each 8 neighbourhood
	uchar roiLBP(cv::Mat input);
	
	//Calcula LBP clássico
	cv::Mat LocalBinaryPattern::grayImageLBP(cv::Mat grayImage);
	
	// DEPRECATED Tenta implementar diferença no LBP (fail)
	cv::Mat LocalBinaryPattern::differenceLBP(cv::Mat depthImage);
	
	//Implementa 3DLBP do paper
	cv::Mat LocalBinaryPattern::TDLBP(cv::Mat depthImage);

	//Testa com imagem da webcam
	void LocalBinaryPattern::webCamTest();

	//Recebe imagem do LBP clássico pra gerar um histograma
	double** LocalBinaryPattern::getHistogram(cv::Mat lbpImage);
public: 
	int LocalBinaryPattern::test();
	cv::Mat LocalBinaryPattern::grayLBPpipeline(cv::Mat frame);
	void LocalBinaryPattern::depthPipeline(cv::Mat frame);


};
