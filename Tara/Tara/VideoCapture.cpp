#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

int capture() {

	VideoCapture vcap(0);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G')/*-1*/, 10, Size(frame_width, frame_height), true);
	typedef std::chrono::high_resolution_clock Clock;
	time_t t1 = time(0);
	int framecount = 0;
	for (;;) {
		Mat frame;
		vcap >> frame;
		video.write(frame);
		framecount++;
		time_t t2 = time(0);
		int framerate = framecount / difftime(t2, t1);
		//cout << "Framerate: " << framerate << endl;
		putText(frame, "FPS: " + to_string(framerate), Point(10, 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		imshow("Frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
	destroyAllWindows();
	return 0;
}