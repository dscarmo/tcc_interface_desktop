#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "VideoCapture.hpp"
#include <iostream>
#include <chrono>
#include <windows.h>

using namespace std;
using namespace cv;

//Captures only gray without Tara capture
int DisparityWriter::syncCapture() {
	Mat channels[3];
	vector<Mat> video1;
	vector<Mat> video2;
	Mat frame, concat;
	int framecount = 0;
	int howlong = 0;

	bool revert;
	double FPS = 30;

	VideoCapture vcap(0);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	//VideoWriter video1("channel1.avi", -1, 25, Size(frame_width, frame_height), true);

	//VideoWriter video2("channel2.avi", -1, 25, Size(frame_width, frame_height), false);

	//VideoWriter recorder("drone_video.avi", -1, FPS, Size(frame_width, frame_height), false);
	VideoWriter recorder1("drone_video_left.avi", CV_FOURCC('I', 'Y', 'U', 'V'), FPS, Size(frame_width, frame_height), false);
	VideoWriter recorder2("drone_video_right.avi", CV_FOURCC('I', 'Y', 'U', 'V'), FPS, Size(frame_width, frame_height), false);
	
	//typedef std::chrono::high_resolution_clock Clock;
	//time_t t1 = time(0);
	cout << "How long should be recorded? Aproximadamente 1 GB de RAM por minuto necessário (segundos)" << endl;
	cin >> howlong;

	revert = false;
	string opt = "";
	while (opt.compare("s") != 0 && opt.compare("n") != 0)
	{
		cout << "Revert image? (s, n)" << endl;
		cin >> opt;
	}


	if (opt.compare("s") == 0)
		revert = true;
	else
		revert = false;

	cout << "Gravando..." << endl;
	for (int l = 0;l < howlong*FPS; l++) {
		vcap >> frame;
		split(frame, channels);
		video1.push_back(channels[1].clone());
		video2.push_back(channels[2].clone());
		framecount++;
		
		if (l % (int)FPS == 0) {
			cout << l / FPS << endl;
		}

		hconcat(channels[1], channels[2], concat);
		imshow("Gravando...", concat);
		waitKey((1 / FPS) * 1000);
	}
	destroyAllWindows();

	cout << "Recording..." << endl;
	Mat fliped1 = Mat(video1[0].rows, video1[0].cols, CV_8UC1);
	Mat fliped2 = Mat(video2[0].rows, video2[0].cols, CV_8UC1);
	for (int i = 0; i < framecount; i++) {
		if (revert) {
			flip(video1[i], fliped1, 0);
			flip(video2[i], fliped2, 0);
			recorder1.write(fliped1);
			recorder2.write(fliped2);
		}
		else
		{
			recorder1.write(video1[i]);
			recorder2.write(video2[i]);
		}
		if (i % (int)FPS == 0) {
			cout << i / FPS << endl;
		}
		//hconcat(video1[i], video2[i], concat);
		//imshow("Codificando arquivo de vídeo...", concat);
		//waitKey(1);
	}
	destroyAllWindows();
	return 0;
}

void getSize(int* width, int* height) {
	VideoCapture vcap(0);
	*width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	*height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << *width << " " << *height << endl;
	Mat test_frame;
	vcap >> test_frame;
	imshow("frame de teste", test_frame);
	waitKey(0);
	vcap.release();
}

//Writes from tara capture sync
void DisparityWriter::syncWriteAll(Mat disparity, Mat left, Mat right) {
	if (revert) {
		flip(disparity, fliped, 0);
		disparityRecorder.write(fliped);

		flip(left, fliped, 0);
		leftRecorder.write(fliped);

		flip(right, fliped, 0);
		rightRecorder.write(fliped);
	}
	else
	{
		disparityRecorder.write(disparity);
		leftRecorder.write(left);
		rightRecorder.write(right);
	}
}

/*
void DisparityWriter::asyncCapLeft0() {
	VideoWriter asyncLeftRecorder("drone_left_async.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, Size(640, 480), false);
	Mat fliped;
	while (true) {
		if (unleashThread[0]) {
			cout << "Gravando Left" << endl;
			if (asyncRevert)
			{
				flip(leftAsync, fliped, 0);
				asyncLeftRecorder.write(fliped);
			}
			else
			{
				asyncLeftRecorder.write(rightAsync);
			}
			unleashThread[0] = false;
		}
		this_thread::yield;
	}
}

void DisparityWriter::asyncCapRight1() {
	VideoWriter asyncRightRecorder("drone_right_async.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, Size(640,480), false);
	Mat fliped;
	while (true) {
		if (unleashThread[1]) {
			cout << "Gravando right" << endl;
			if (asyncRevert)
			{
				flip(rightAsync, fliped, 0);
				asyncRightRecorder.write(fliped);
			}
			else
			{
				asyncRightRecorder.write(rightAsync);
			}
			unleashThread[1] = false;
		}
		this_thread::yield;
	}
}

void DisparityWriter::asyncCapDisp2() {
	VideoWriter asyncDispRecorder("drone_disp_async.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, Size(640, 480), false);
	Mat fliped;
	while (true) {
		if (unleashThread[2]) {
			cout << "Gravando disp" << endl;
			if (asyncRevert)
			{
				flip(dispAsync, fliped, 0);
				asyncDispRecorder.write(fliped);
			}
			else
			{
				asyncDispRecorder.write(rightAsync);
			}
			unleashThread[2] = false;
		}
		this_thread::yield;
	}
}

void DisparityWriter::asyncManager(cv::Mat disparity, cv::Mat left, cv::Mat right) {
	leftAsync = left.clone();
	rightAsync = right.clone();
	dispAsync = disparity.clone();

	unleashThread[0] = true;
	unleashThread[1] = true;
	unleashThread[2] = true;
}*/