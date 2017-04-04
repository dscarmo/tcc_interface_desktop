#include "stdafx.h"
#include "FaceIdentification.hpp"
#include "FaceDetection.hpp"
#include "LocalBinaryPattern.hpp"
#include "VideoCapture.hpp"
#include <ctime>

using namespace std;
using namespace cv;
int main() {
	int choice;
	//LocalBinaryPattern lbp;
	//lbp.test();
	//cin >> choice;
	//return 0;
	//captureAndDetect();
	//int choice;
	cout << "Stereo Dataset Recorder ou Face Identification? (0, 1)";
	cin >> choice;

	if (choice) {
		//Face detect
		int ReturnStatus = -1;

		//Object creation
		FaceIdentification _FaceIdentification;

		//Initialises the Face Detection Project
		ReturnStatus = _FaceIdentification.Init();

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
