#include "stdafx.h"
#include "DepthViewer.h"
#include "VideoCapture.hpp"
#include "FaceDepth.h"
#include "FaceDetection.hpp"

#include <ctime>

using namespace std;
using namespace cv;
int main() {
	//captureAndDetect();
	int choice;
	cout << "Disparity Recorder ou Face Detection? (0, 1)";
	cin >> choice;

	if (choice) {
		//Face detect
		int ReturnStatus = -1;
		PrintDebug(DEBUG_ENABLED, L"Face Detection!");

		//Object creation
		FaceDepth _FaceDepth;

		//Initialises the Face Detection Project
		ReturnStatus = _FaceDepth.Init();

		PrintDebug(DEBUG_ENABLED, L"Exit: Face Detection!");
		cout << endl << "Exit: Face Detection Application" << endl << endl;

		if (!ReturnStatus) //check for a valid return
		{
			cout << endl << "Press any key to exit!" << endl << endl;
			_getch();
		}
		return 0;
	}
	else {
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
		return 0;
	}
}
