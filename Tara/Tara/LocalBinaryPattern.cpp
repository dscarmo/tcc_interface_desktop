#include <iostream>
#include "stdafx.h"
#include "LocalBinaryPattern.hpp"
#include "Utils.hpp"


using namespace std;
using namespace cv;

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

			//if (!isfinite(hist[h][w]))
				//hist[h][w] = 0;

			//std::cout << hist[h][w] << "\n";
		}
	img_temp.release();
	return hist;
}


void drawHist(vector<double>& data, Mat3b& dst, int binSize = 3, int height = 0)
{
	int max_value = *max_element(data.begin(), data.end());
	int rows = 0;
	int cols = 0;
	if (height == 0) {
		rows = max_value + 10;
	}
	else {
		rows = max(max_value + 10, height);
	}

	cols = data.size() * binSize;

	dst = Mat3b(rows, cols, Vec3b(0, 0, 0));

	for (int i = 0; i < data.size(); ++i)
	{
		int h = rows - data[i];
		rectangle(dst, Point(i*binSize, h), Point((i + 1)*binSize - 1, rows), (i % 2) ? Scalar(0, 100, 255) : Scalar(0, 0, 255), CV_FILLED);
	}

}

//Tests computing LBP with webcam
void LocalBinaryPattern::webCamTest() {
	VideoCapture vcap(0);
	timer = Timer();
	Mat frame, grayframe;
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return;
	}

	while (true) {
		timer.reset();
		vcap >> frame;
		cout << "Frame capture time: " << timer.elapsed() << "s" << endl;

		timer.reset();
		cvtColor(frame, grayframe, CV_BGR2GRAY);
		cout << "Color conversion time: " << timer.elapsed() << "s" << endl;
		//grayLBPpipeline(grayframe);
		Timer wholeTime = Timer();
		wholeTime.reset();
		betterLBPpipeline(grayframe);
		cout << "Image to feature time: " << wholeTime.elapsed() << endl;
		waitKey(1);
	}
}

//Gets LBP values and histogram of provided ROI for LBP
Mat LocalBinaryPattern::grayLBPpipeline(Mat frame) {
	double** histograma;
	vector<double> hist; //container do vetor de debug
	Mat lbp; //imagem lbp
	Mat3b draw; //container do draw
	//imshow("frame", frame);
	lbp = grayImageLBP(frame);
	imshow("lbp", lbp);
	histograma = getHistogram(lbp);
	
	//Debug draw
	double acum = 0;
	for (int i = 0; i < 256; i++)
	{
		double frequency = histograma[0][i];
		//cout << i << ": " << frequency << endl;
		acum += frequency;
		hist.push_back(frequency*255);
	}
	drawHist(hist, draw);
	imshow("histograma lbp", draw);
	//cout << acum << endl;
	hist.clear();

	Mat histograma_temp;
	histograma_temp = Mat::zeros(Size(1, 256), CV_32F);
	for (unsigned int i = 0; i < 256; i++)
		histograma_temp.at<float>(i, 0) = histograma[0][i];

	Mat histograma_mat = histograma_temp.clone();
	delete[] histograma[0];
	delete[] histograma;

	Mat transposta;
	transpose(histograma_mat, transposta);

	return transposta;
}

Mat LocalBinaryPattern::LBP(Mat img) {
	Mat dst = Mat::zeros(img.rows - 2, img.cols - 2, CV_8UC1);
	for (int i = 1; i<img.rows - 1; i++) {
		for (int j = 1; j<img.cols - 1; j++) {
			uchar center = img.at<uchar>(i, j);
			unsigned char code = 0;
			code |= ((img.at<uchar>(i - 1, j - 1)) > center) << 7;
			code |= ((img.at<uchar>(i - 1, j)) > center) << 6;
			code |= ((img.at<uchar>(i - 1, j + 1)) > center) << 5;
			code |= ((img.at<uchar>(i, j + 1)) > center) << 4;
			code |= ((img.at<uchar>(i + 1, j + 1)) > center) << 3;
			code |= ((img.at<uchar>(i + 1, j)) > center) << 2;
			code |= ((img.at<uchar>(i + 1, j - 1)) > center) << 1;
			code |= ((img.at<uchar>(i, j - 1)) > center) << 0;
			dst.at<uchar>(i - 1, j - 1) = code;
		}
	}
	return dst;
}

Mat LocalBinaryPattern::LBPHistogram(Mat lbp) {
	Mat histogram = Mat::zeros(Size(256, 1), CV_32F);

	for (int y = 0; y < lbp.rows; y++)
		for (int x = 0; x < lbp.cols; x++)
		{
			int bin = lbp.at<uchar>(y, x);
			histogram.at<float>(0, bin) = histogram.at<float>(0, bin) + 1;
		}

	//cout << "Área do LBP: " << lbp.size().area() << endl;
	//cout << "Soma do histograma não normalizado: " << sum(histogram)[0] << endl;
	// manual normalization
	for (int z = 0; z < histogram.cols; z++)
		histogram.at<float>(0, z) = histogram.at<float>(0, z) / lbp.size().area();
	//cout << "Soma do histograma normalizado: " << sum(histogram)[0] << endl;

	return histogram;
}

//Gets LBP values and histogram of provided ROI for LBP
Mat LocalBinaryPattern::betterLBPpipeline(Mat frame) {
	resize(frame, frame, Size(192, 168));
	Mat feature = Mat::zeros(Size(FEATURE_SIZE,1), CV_32F);;
	
	vector<Mat> lbps;
	Mat lbp; //imagem lbp


	lbp = LBP(frame);
	imshow("whole frame lbp", lbp);
	
	int N = 16;
	int index = 0;
	timer.reset();
	for (int r = 0; r < frame.rows; r += N)
		for (int c = 0; c < frame.cols; c += N)
		{
			Mat tile = frame(cv::Range(r, min(r + N, frame.rows)), cv::Range(c, min(c + N, frame.cols)));
			Mat tile_histogram = LBPHistogram(LBP(tile));
			for (int i = 0; i < tile_histogram.cols; i++)
			{
				feature.at<float>(0, i + index) = tile_histogram.at<float>(0, i);
			}
			index += 256;
		}
	//cout << "LBP Feature construction time: " << timer.elapsed() << endl;

	//Debug draw double vector
	vector<double> hist; //container do vetor de debug
	Mat3b draw; //container do draw
	timer.reset();
	for (int i = 0; i < feature.cols; i++)
	{
		double frequency = feature.at<float>(0,i);
		hist.push_back(frequency*100);
	}
	drawHist(hist, draw);
	imshow("histograma lbp", draw);
	waitKey(1);
	//cout << "Debug histogram display time: " << timer.elapsed() << endl;
	return feature;
}

