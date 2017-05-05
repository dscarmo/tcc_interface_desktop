#include "stdafx.h"
#include <ctime>
#include "FaceIdentification.hpp"
#include "FaceDetection.hpp"
#include "LocalBinaryPattern.hpp"
#include "VideoCapture.hpp"
#include "NeuralNetwork.hpp"


using namespace std;
using namespace cv;
int main() {
	int choice;
	//cin >> choice;
	//return 0;
	//captureAndDetect();
	//int choice;
	cout << "Stereo Dataset Recorder, Face Identification, WebCam Test or Run from Files?? (0, 1, 2, 3)";
	cin >> choice;

	if (choice == 1) {
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
			system("PAUSE");
		}
		return 0;
	}
	else if (choice == 0) {
		syncCapture();

		return 0;
		
	}
	else if (choice == 2){
		LocalBinaryPattern lbp;
		lbp.test();


	}
	else if (choice == 3) {
		testMSN();
		//runFromDirectory();
	}
}
