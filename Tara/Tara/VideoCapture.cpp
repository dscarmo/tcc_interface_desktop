#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

int capture() {
	int FPS = 25;
	Mat channels[3];
	VideoCapture vcap(0);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH)*2;
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	//VideoWriter video1("channel1.avi", -1, 25, Size(frame_width, frame_height), true);

	//VideoWriter video2("channel2.avi", -1, 25, Size(frame_width, frame_height), false);

	VideoWriter recorder("drone_video.avi", -1, FPS, Size(frame_width, frame_height), false);
	typedef std::chrono::high_resolution_clock Clock;
	time_t t1 = time(0);
	int framecount = 0;
	for (;;) {
		Mat frame, concat;
		vcap >> frame;
		split(frame, channels);
		hconcat(channels[1], channels[2], concat);
		imshow("concatenado", concat);
		recorder.write(concat);
		framecount++;
		time_t t2 = time(0);
		int framerate = framecount / difftime(t2, t1);
		cout << "Framerate: " << framerate << endl;
		//putText(frame, "FPS: " + to_string(framerate), Point(10, 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		if (waitKey(1) == 27) 
			break;
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
/*
//DEPRECATED
int record_frame(Mat frame, string name, int width, int height) {
	VideoWriter video(name, CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(width, height), true);
	typedef std::chrono::high_resolution_clock Clock;
	static time_t t1 = time(0);
	static int framecount = 0;
	try
	{
		video.write(frame);
	}
	catch (const std::exception &e)
	{
		cerr << e.what();
	}
	framecount++;
	static time_t t2 = time(0);
	int framerate = framecount / difftime(t2, t1);
	//cout << "Framerate: " << framerate << endl;
	putText(frame, "FPS: " + to_string(framerate), Point(10, 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	imshow(name, frame);
	

	return 0;
}*/