#include "stdafx.h"
#include "LocalBinaryPattern.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//Not useful
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

//This makes no sense
// Calcula LBP direto de uma imagem de profundidade
Mat LocalBinaryPattern::differenceLBP(Mat depthImage) {
	//Aplicando Construtor (ros, cols, type)
	Mat dst(depthImage.rows - 2, depthImage.cols - 2, CV_8UC1);
	dst = Scalar(0);

	int difference = 0;

	//Compute normal LBP
	for (int i = 1; i<depthImage.rows - 1; i++) {
		for (int j = 1; j<depthImage.cols - 1; j++) {
			unsigned char code = 0;
			code |= (depthImage.at<uchar>(i - 1, j - 1) > depthImage.at<uchar>(i, j)) << 7;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i - 1, j - 1));

			code |= (depthImage.at<uchar>(i - 1, j) > depthImage.at<uchar>(i, j)) << 6;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i - 1, j));

			code |= (depthImage.at<uchar>(i - 1, j + 1) > depthImage.at<uchar>(i, j)) << 5;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i - 1, j + 1));

			code |= (depthImage.at<uchar>(i, j + 1) > depthImage.at<uchar>(i, j)) << 4;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i, j + 1));

			code |= (depthImage.at<uchar>(i + 1, j + 1) > depthImage.at<uchar>(i, j)) << 3;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i + 1, j + 1));

			code |= (depthImage.at<uchar>(i + 1, j) > depthImage.at<uchar>(i, j)) << 2;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i + 1, j));

			code |= (depthImage.at<uchar>(i + 1, j - 1) > depthImage.at<uchar>(i, j)) << 1;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i + 1, j - 1));

			code |= (depthImage.at<uchar>(i, j - 1) > depthImage.at<uchar>(i, j)) << 0;
			difference += (int)(depthImage.at<uchar>(i, j) - depthImage.at<uchar>(i, j - 1));

			int total = (difference /*+ (int)code*/);
			if (total > 255) {
				dst.at<unsigned char>(i - 1, j - 1) = 255;
			}
			else if (total < 0) {
				dst.at<unsigned char>(i - 1, j - 1) = 0;
			}
			else
				//Add difference number to final binary number
				dst.at<unsigned char>(i - 1, j - 1) = (unsigned char)total;


			
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
	Mat frame, grayframe;
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return;
	}

	while (true) {
		vcap >> frame;
		cvtColor(frame, grayframe, CV_BGR2GRAY);
		grayLBPpipeline(grayframe);
		depthPipeline(grayframe);
		waitKey(1);
	}
}

//Gets LBP values and histogram of provided ROI for LBP
void LocalBinaryPattern::grayLBPpipeline(Mat frame) {
	double** histogram;
	vector<double> hist;
	Mat lbp;
	Mat3b draw;
	imshow("frame", frame);
	lbp = grayImageLBP(frame);
	imshow("lbp", lbp);
	histogram = getHistogram(lbp);
	//cout << endl << "Histograma: " << endl;
	double acum = 0;
	for (int i = 0; i < 256; i++)
	{
		double frequency = histogram[0][i];
		//cout << i << ": " << frequency << endl;
		acum += frequency;
		hist.push_back(frequency*255);
	}
	drawHist(hist, draw);
	imshow("histograma lbp", draw);
	cout << acum << endl;
	hist.clear();
}

//Applies depth methods
void LocalBinaryPattern::depthPipeline(Mat frame) {
	//Size frameSize = frame.size();
	//double** histogram;
	//vector<double> hist;
	//HOGDescriptor hog;
	//vector<float> features;
	//vector<Point>locs;
	//Mat3b draw;
	//Mat feature;
	//imshow("3d frame", frame);
	Mat descriptor;
	Mat normalized_descriptor;
	vector<double> hist;
	Mat3b draw;
	try
	{
		descriptor = TDLBP(frame);
		normalize(descriptor, normalized_descriptor, 0, 1, NORM_MINMAX,CV_32FC1);
		double acum = 0;
		for (int i = 0; i < 14 * 64 * 4; i++) //put this size in a define
		{
			double frequency = normalized_descriptor.at<float>(0,i);
			//cout << i << ": " << frequency << endl;
			acum += frequency;
			hist.push_back(frequency*255);
		}
		//cout << acum << endl;
		drawHist(hist, draw);
		imshow("histograma 3dlbp", draw);
		//hog.compute(frame, features);
		//cout << features.size() << endl;
		/*feature.create(ders.size(), 1, CV_32FC1);
		for (int i = 0; i<ders.size(); i++)
		{
			feature.at<float>(i, 0) = ders.at(i);
		}*/
	}
	catch (const std::exception& e)
	{
		cout << e.what();
	}
	
	//lbp3d = differenceLBP(frame);
	//imshow("lbp3d", lbp3d);
	

	/*histogram = getHistogram(frame);
	//cout << endl << "Histograma: " << endl;
	double acum = 0;
	for (int i = 0; i < 256; i++)
	{
		double frequency = histogram[0][i];
		//cout << i << ": " << frequency << endl;
		acum += frequency;
		hist.push_back(frequency * 100);
	}
	drawHist(hist, draw);*/
//	imshow("histograma 3dlbp", draw);
	//cout << acum << endl;
	//hist.clear();
}

Mat LocalBinaryPattern::TDLBP(Mat depthImage) {
	Mat finalDescriptor = calculate3DLBP(depthImage);
	Mat descriptorDisplay;
	finalDescriptor.convertTo(descriptorDisplay, CV_8UC1);
	imshow("3dlbp descriptor",descriptorDisplay);
	return finalDescriptor;
}

int LocalBinaryPattern::test() {
	webCamTest();
	return 0;
	uchar testData[9] = { 4, 3, 4,
						  3, 3, 3,
						  3, 3, 3 };
	Mat testMat(Size(3, 3), CV_8UC1, testData);
	cout << testMat << endl;
	cout << grayImageLBP(testMat);
	//cout << differenceLBP(testMat);

	//Actual intended LBP:
	//Get face ROI
	//Get LBP image
	//Get normalized histogram of LBP image
	//Thats the histogram of the considered region
	return 0;
}
