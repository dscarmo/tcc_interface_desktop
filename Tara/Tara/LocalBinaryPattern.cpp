#include "stdafx.h"
#include "LocalBinaryPattern.hpp"
#include <iostream>

using namespace std;
using namespace cv;

uchar LocalBinaryPattern::roiLBP(cv::Mat input) {

	for(int j = 0; j < input.rows; j++)
		for (int i = 0; i < input.cols; i++) {
			cout << int(input.at<uchar>(j, i));
		}
	return 0;
}

//Calcula LBP direto de uma imagem cinza
Mat LocalBinaryPattern::grayImageLBP(Mat grayImage) {
	//Aplicando Construtor (ros, cols, type)
	Mat dst(grayImage.rows - 2, grayImage.cols - 2, CV_8UC1);
	dst = Scalar(0);

	for (int i = 1; i<grayImage.rows - 1; i++) {
		for (int j = 1; j<grayImage.cols - 1; j++) {
			unsigned char code = 0;
			code |= (grayImage.at<uchar>(i - 1, j - 1) > grayImage.at<uchar>(i, j)) << 7;
			code |= (grayImage.at<uchar>(i - 1, j) > grayImage.at<uchar>(i, j)) << 6;
			code |= (grayImage.at<uchar>(i - 1, j + 1) > grayImage.at<uchar>(i, j)) << 5;
			code |= (grayImage.at<uchar>(i, j + 1) > grayImage.at<uchar>(i, j)) << 4;
			code |= (grayImage.at<uchar>(i + 1, j + 1) > grayImage.at<uchar>(i, j)) << 3;
			code |= (grayImage.at<uchar>(i + 1, j) > grayImage.at<uchar>(i, j)) << 2;
			code |= (grayImage.at<uchar>(i + 1, j - 1) > grayImage.at<uchar>(i, j)) << 1;
			code |= (grayImage.at<uchar>(i, j - 1) > grayImage.at<uchar>(i, j)) << 0;
			dst.at<unsigned char>(i - 1, j - 1) = code;
		}
	}

	return dst;
}


double** LocalBinaryPattern::getHistogram(cv::Mat lbpImage) {
	double** hist = 0;
	hist = new double*[1];

	for (int h = 0; h < 1; h++)
	{
		hist[h] = new double[256];

		for (int w = 0; w < 256; w++)
		{
			hist[h][w] = 0;
		}
	}

	Mat img_temp(lbpImage);

	//Já obtendo o histograma de um canal só, que é como fazer no Matlab 
	//A função imhist(im(:))

	for (int i = 0; i < img_temp.rows; i++) {
		for (int j = 0; j < img_temp.cols; j++)
		{
			int pixel = img_temp.at<uchar>(i, j);

			//No "pixel" incrementa a frequencia
			hist[0][pixel] = hist[0][pixel] + 1;
			//canal B
		}

	}

	//Normalizando o histograma?
	for (int h = 0; h < 1; h++)
		for (int w = 0; w < 256; w++) {
			hist[h][w] = double(hist[h][w]) / (img_temp.rows*img_temp.cols);

			if (!isfinite(hist[h][w]))
				hist[h][w] = 0;

			//std::cout << hist[h][w] << "\n";
		}

	img_temp.release();
	return hist;
}

int LocalBinaryPattern::test() {
	uchar testData[9] = { 3, 4, 5,
						  0, 3, 0,
						  1, 2, 3 };
	Mat testMat(Size(3, 3), CV_8UC1, testData);
	cout << testMat << endl;
	cout << grayImageLBP(testMat);

	//Actual intended LBP:
	//Get face ROI
	//Get LBP image
	//Get normalized histogram of LBP image
	//Thats the histogram of the considered region
	return 0;
}