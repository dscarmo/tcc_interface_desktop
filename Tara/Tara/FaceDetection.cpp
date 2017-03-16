#include "stdafx.h"
#include "FaceDetection.hpp"
#include <ctime>

using namespace std;
using namespace cv;

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
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

/** @function main */
int captureAndDetect()
{
	VideoCapture capture(0);
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
void faceDetect(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces);

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame, faces[i], Scalar(255, 0, 255));

		// 20% increase in given rect
		//Mat faceROI = frame_gray(Rect(0.8*faces[i].x, 0.8*faces[i].y, 1.44*faces[i].width, 1.44*faces[i].height));

		Mat faceROI = frame_gray(faces[i]);


		imshow("ROI", faceROI);
	}
}
