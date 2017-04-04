#include "stdafx.h"
#include "FaceDetection.hpp"
#include <ctime>

using namespace std;
using namespace cv;

/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
CascadeClassifier face_cascade;

//Draw FPS utility
// --draws fps in a given image, using frequency of calls
Mat drawFps(Mat input) {
	Mat output = input.clone();
	static clock_t last_time;
	static int executedOnce;
	float dif;
	if (!executedOnce) {
		dif = 0;
		executedOnce = 1;
	}
	else {
		dif = 1 / (float(clock() - last_time) / CLOCKS_PER_SEC);
		putText(output, to_string(int(dif)) + " FPS", Point(50, 50), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	}
	last_time = clock();
	return output;
}

int initCascade() {
	if (!face_cascade.load(face_cascade_name)) { 
		printf("--(!)Error loading\n"); 
		return 1; 
	}
	else return 0;
}

/** @function main */
int testFaceDetection()
{
	int cam = 1;
	int web_width = 320;
	int web_height = 240;
	VideoCapture capture(cam);
	if (cam) {
		capture.set(CV_CAP_PROP_FRAME_WIDTH, web_width);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, web_height);
	}
	//
	Mat frame;

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	//-- 2. Read the video stream
	if (capture.isOpened())
	{
		bool enableFace = true;
		while (true)
		{
			capture >> frame;

			//-- 3. Apply the classifier to the frame
			if (enableFace)
			{
				if (!frame.empty())
				{
					faceDetect(frame);
				}
				else
				{
					printf(" --(!) No captured frame -- Break!"); break;
				}
			}

			//-- Show what you got
			imshow("Face Detection", drawFps(frame));

			int c = waitKey(1);
			if ((char)c == 'c') { break; }
			else if ((char)c == 'e') {
				destroyAllWindows();
				enableFace = !enableFace;
			}

		}
	}
	return 0;
}

/** @function detectAndDisplay */
vector<Rect> faceDetect(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	equalizeHist(frame, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces);

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame, faces[i], Scalar(255, 0, 255));
		Mat faceROI = frame_gray(faces[i]);
	}

	return faces;
}
