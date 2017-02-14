#pragma once

int capture();
void getSize(int* width, int* height);


class DisparityWriter {
private:
	//Initializes a writer for disparity
	cv::VideoWriter disparityRecorder;
	cv::Size size;
	int FPS;
	bool revert;
	cv::Mat fliped;

public:
	DisparityWriter() {}
	DisparityWriter(cv::Size sizee, int FPSS, bool revertt) {
		size = sizee;
		FPS = FPSS;
		revert = revertt;
		disparityRecorder = cv::VideoWriter("drone_disparity.avi", CV_FOURCC('I', 'Y', 'U', 'V'), FPS, size, true);
		fliped = cv::Mat(sizee.height, sizee.width, CV_8UC3);
	}

	//Is called multiple times
	void storeDisparity(cv::Mat disparity) {
		if (revert) {
			flip(disparity, fliped, -1);
			disparityRecorder.write(fliped);
		}
		else
		{
			disparityRecorder.write(fliped);
		}
	}
};