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


int capture() {
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
	VideoWriter recorder1("drone_video_1.avi", CV_FOURCC('I', 'Y', 'U', 'V'), FPS, Size(frame_width, frame_height), false);
	VideoWriter recorder2("drone_video_2.avi", CV_FOURCC('I', 'Y', 'U', 'V'), FPS, Size(frame_width, frame_height), false);
	
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
			flip(video1[i], fliped1, -1);
			flip(video2[i], fliped2, -1);
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


	
//Is called multiple times
void DisparityWriter::storeDisparity(Mat disparity) {
	if (revert) {
		flip(disparity, fliped, -1);
		disparityRecorder.write(fliped);
	}
	else
	{
		disparityRecorder.write(disparity);
	}
}
