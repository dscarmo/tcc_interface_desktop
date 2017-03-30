#include "stdafx.h"
#include "FaceDepth.h"
#include "FaceDetection.hpp"
#include "LocalBinaryPattern.hpp"
#include "VideoCapture.hpp"
#include <ctime>

using namespace std;
using namespace cv;
int main() {

	//LocalBinaryPattern lbp;
	//return lbp.test();
	//captureAndDetect();
	int choice;
	cout << "Stereo Dataset Recorder ou Face Identification? (0, 1)";
	cin >> choice;

	if (choice) {
		//Face detect
		int ReturnStatus = -1;

		//Object creation
		FaceDepth _FaceDepth;

		//Initialises the Face Detection Project
		ReturnStatus = _FaceDepth.Init();

		cout << endl << "Exit: Face Detection Application" << endl << endl;

		if (!ReturnStatus) //check for a valid return
		{
			cout << endl << "Press any key to exit!" << endl << endl;
			_getch();
		}
		return 0;
	}
	else {
		DisparityWriter disparityWriter;

		disparityWriter.syncCapture();

		return 0;
		/*
		//Disparity Recorder
		PrintDebug(DEBUG_ENABLED, L"Depth Viewer");
		int ReturnStatus = -1;

		//Object creation
		DepthViewer _DepthViewer;

		//Initialises the depth view
		ReturnStatus = _DepthViewer.Init();

		PrintDebug(DEBUG_ENABLED, L"Exit: Depth Viewer");
		cout << endl << "Exit: Depth Viewer" << endl << endl;

		if (!ReturnStatus) //check for a valid return
		{
			cout << endl << "Press any key to exit!" << endl << endl;
			_getch();
		}
		return 0;*/
	}
}
