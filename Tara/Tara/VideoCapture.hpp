#pragma once
#include <thread>

void getSize(int* width, int* height);

//Async vars
//bool unleashThread[3] = { false, false, false };
//cv::Mat leftAsync;
//cv::Mat rightAsync;
//cv::Mat dispAsync;
//bool asyncRevert;
class DisparityWriter {
private:
	//Initializes a writer for disparity
	cv::VideoWriter disparityRecorder;
	cv::VideoWriter leftRecorder;
	cv::VideoWriter rightRecorder;
	cv::Size size;
	int FPS;
	bool revert;
	cv::Mat fliped;
	cv::Mat asyncFlip[3];
	//std::thread threadedWriteL;
	//std::thread threadedWriteR;
	//std::thread threadedWriteD;

public:
	//Async write
	
	

	bool setupDone = false;
	DisparityWriter() {}
	DisparityWriter(cv::Size sizee, int FPSS, bool revertt) {
		size = sizee;
		FPS = FPSS;
		revert = revertt;
		//asyncRevert = revertt;
		disparityRecorder = cv::VideoWriter("drone_disparity.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, cv::Size(802,480), true);
		leftRecorder = cv::VideoWriter("drone_video_left.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, cv::Size(752, 480), false);
		rightRecorder = cv::VideoWriter("drone_video_right.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 10, cv::Size(752, 480), false);
		fliped = cv::Mat(sizee.height, sizee.width, CV_8UC3);
		setupDone = true;
		//threadedWriteL = std::thread(asyncCapLeft0);
		//threadedWriteR = std::thread(asyncCapRight1);
		//threadedWriteD = std::thread(asyncCapDisp2);
	}

	//void asyncManager(cv::Mat disparity, cv::Mat left, cv::Mat right);

	//static void asyncCapLeft0();

	//static void asyncCapRight1();

	//static void asyncCapDisp2();

	void syncWriteAll(cv::Mat disparity, cv::Mat left, cv::Mat right);

	int syncCapture();
};