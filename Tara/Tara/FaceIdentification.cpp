///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, e-Con Systems.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT/INDIRECT DAMAGES HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/**********************************************************************
	FaceIdentification: Estimates the depth of the person from the camera by 
			    detecting the face of the person
**********************************************************************/
#include "stdafx.h"
#include "FaceIdentification.hpp"
#include "FaceDetection.hpp"
#include "LocalBinaryPattern.hpp"
#include "VideoCapture.hpp"
#include "multipleSmallNetworks.hpp"
#include <iomanip>
#include <ctime>
#include <algorithm>

#define RIGHTMATCH 150 //disparity range starts from 150 so in case of the point being less than 150 it means that the face is not fully covered in the right.
#define CAM 0
#define FILE 1
#define RECORD 1
//Constructor
FaceIdentification::FaceIdentification(void)
{
	ss.precision(2); //Restricting the precision after decimal
}

//Initialises all the variables and methods
int FaceIdentification::Init()
{	
	cout << endl << "		Face Detection Application " << endl  << endl;
	cout << " Detects multiple faces in the frame!" << endl << " Uses OpenCV Haarcascade file in the Face folder!" << endl;
	cout << " Rectangle around the detected faces with the depth is displayed!" << endl << " Only the left window is displayed!" << endl << endl;


	if(!_Disparity.InitCamera(true, true)) //Initialise the camera
	{
		Tara::PrintDebug(DEBUG_ENABLED, L"Camera Initialisation Failed!");
		return 0;
	}

	//Streams the camera and process the height
	CameraStreaming();

	return 1;
}

//Unscaling the point to the actual image size for the lower resolutions
Point FaceIdentification::unscalePoint(Point Pt, Size CurrentSize, Size TargetSize)
{
	Point DesPt;

	DesPt.x = int( (Pt.x /(float) CurrentSize.width)  * TargetSize.width );
	DesPt.y = int( (Pt.y /(float) CurrentSize.height) * TargetSize.height);
 
	return DesPt;
}



Mat faceRectangle;

void onMouse(int event, int x, int y, int, void*)
{
	if (event != CV_EVENT_LBUTTONDOWN)
		return;
	if (!faceRectangle.empty())
		cout << "x: " << x << "- y: " << y << endl << int(faceRectangle.at<uchar>(y, x));
}
//Streams the input from the camera
int FaceIdentification::CameraStreaming()
{	
	stringstream ss;
	vector<Rect> LFaces;
	Rect RFace;
	float DepthValue;
	bool RImageDisplay = false;
	int BrightnessVal = 4;		//Default value
	Mat gDisparityMap, gDisparityMap_viz, RightImage, DisplayImage, DepthDisplayImage, depthMap, leftRegion, rightRegion, leftDetect, finalDebug;
	Point g_SelectedPoint;
	Rect saveRect(20,20, 120, 120);
	//user key input
	char WaitKeyStatus;

	//Window Creation
	//namedWindow("Left Image", WINDOW_AUTOSIZE);
	
	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" <<endl;
	cout << endl << "Press r/R on the Image Window to see the right image" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl << endl;

	//cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;
	bool save = false;
	int saved = 0;

	if (initCascade()) 
	{
		cout << "Detection file problem!";
		return 1;
	}


	//Record for dataset
	//_Disparity.GrabFrame(&LeftImage, &RightImage);
	//DisparityWriter dw(LeftImage.size(), 5, false);
	int option1, option2;
	option2 = -1;

	string idString;

	option1 = CAM;

	VideoCapture left_cap;
	VideoCapture right_cap;
	VideoCapture depth_cap;
	
	MSN *msn = new MSN();
	destroyAllWindows();
	//Estimates the depth of the face using Haarcascade file of OpenCV

	//Detect the faces
	Rect face(20, 20, 120, 120);
	while(1)
	{
		if (option1 == CAM) {
			if (!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
			{
				destroyAllWindows();
				break;
			}

			//Get disparity
			//_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);
		}
		
		try
		{
			DisplayImage = LeftImage.clone();
			//cvtColor(gDisparityMap_viz, DepthDisplayImage, CV_BGR2GRAY);
			//DepthDisplayImage = gDisparityMap.clone();
			LFaces = faceDetect(DisplayImage);
		
			//Normalizer roi
			Rect roi(20, 20, 60, 70);
			Mat preEq, nnInput, dInput;
			if (LFaces.size() > 0)
			{
				//Adjust approximate face region 
				RFace = Rect(max<int>(LFaces[0].x - 80, 0), max<int>(LFaces[0].y - 70, 0), LFaces[0].width + 80, LFaces[0].height + 110);
				if (RFace.br().x > DisplayImage.size().width || RFace.br().y > DisplayImage.size().height) { RFace.height = LFaces[0].height; RFace.width = LFaces[0].width; }
				
				//Detection and approximate face regions for disparity calculations
				leftDetect = DisplayImage(LFaces[0]);
				rightRegion = RightImage(RFace);
				leftRegion = LeftImage(RFace);
				_Disparity.GetDisparity(leftRegion, rightRegion, &gDisparityMap, &gDisparityMap_viz);

				imshow("detected rect", leftDetect);
				imshow("approximate face region on right", rightRegion);
				imshow("approximate face region on left", leftRegion);

				imshow("visualização depth", gDisparityMap_viz);

				imshow("disparity crua", gDisparityMap);
				int i = 0;
				for (Rect lface : LFaces) 
				{
					//Neural recognition
					if (lface.width >= 100 && lface.height >= 100) 
					{
						//resize(leftDetect, preEq, Size(100, 100));
						//preEq = preEq(roi);
						//Percentage roi
						preEq = leftDetect(Rect(Point(leftDetect.size().width*0.15, leftDetect.size().height*0.2), //top-left
												Point(leftDetect.size().width*0.85, leftDetect.size().height*0.9))); //bottom-right
															

						resize(preEq, preEq, Size(DEFAULT_DATASET_WIDTH, DEFAULT_DATA_HEIGHT));
						equalizeHist(preEq, nnInput);
						imshow("live pre processing result", nnInput);
						idString = msn->identificate(nnInput, dInput);
						waitKey(1);
					}

					//Save last detected face
					if (i == (LFaces.size() - 1)) {
						face = LFaces[i];
					}

					//Pointing to the center of the face
					g_SelectedPoint = Point(LFaces[i].x + LFaces[i].width / 2, LFaces[i].y + LFaces[i].height / 2);

					Point scaledPoint = unscalePoint(g_SelectedPoint, LeftImage.size(), gDisparityMap.size());

					//Find the depth of the point passed
					_Disparity.EstimateDepth(scaledPoint, &DepthValue);

					//Construct debug image
					DisplayImage = drawFps(LeftImage);
					rectangle(DisplayImage, LFaces[0], Scalar(0, 0, 255));
					rectangle(DisplayImage, RFace, Scalar(255, 255, 255));
					rectangle(RightImage, RFace, Scalar(255, 255, 255));
					putText(DisplayImage, idString, Point(LFaces[i].x + LFaces[i].height, LFaces[i].y), FONT_HERSHEY_PLAIN, 1, Scalar(100, 100, 255));

					if (g_SelectedPoint.x > RIGHTMATCH && DepthValue > 0) //Mark the point selected by the user
					{
						ss << DepthValue / (10 * 2) << " cm\0";
						Tara::DisplayText(DisplayImage, ss.str(), Point(LFaces[i].x, LFaces[i].y));
						ss.str(string());
					}

					i++;
				}

				//Save if save toggled
				if (save) {
					Mat saveimg;
					struct tm newtime;
					time_t now = time(0);
					localtime_s(&newtime, &now);
					string date = to_string(newtime.tm_hour) + to_string(newtime.tm_min) + to_string(newtime.tm_sec);
					//resize(LeftImage(LFaces[0]), saveimg, Size(100, 100));
					if (!preEq.empty())
						imwrite("Faces\\" + date + ".jpg", preEq);
				}
			}
		}
		catch (const std::exception& e)
		{
			cout << e.what();
			continue;
		}
		
		hconcat(DisplayImage, RightImage, finalDebug);
		imshow("Debug",  finalDebug);			

		//Mouse and keyboard callbacks
		setMouseCallback("retanguloFace", onMouse, 0);

		//Command events:
		WaitKeyStatus = waitKey(1);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			//if (option2 == RECORD) dw.syncRecordAll();
			vector<Rect>().swap(LFaces);
			destroyAllWindows();
			break;
		}

		else if (WaitKeyStatus == 's' || WaitKeyStatus == 'S') //Show the right image
		{
			cout << "Saving next detected faces" << endl;
			save = !save;
			waitKey(0);
		}
		else if(WaitKeyStatus == 'b' || WaitKeyStatus == 'B') //Brightness
		{	
			cout << endl << "Enter the Brightness Value, Range(1 to 7): " << endl;
						
			BrightnessVal = 0;			
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats and Alphanumeric strings
			{			
				std::stringstream ss(Inputline);
				if (ss >> BrightnessVal)
				{					
					if (ss.eof())
					{  						
						//Setting up the brightness of the camera
						if (BrightnessVal >= 1  && BrightnessVal <= 7)
						{
							//Setting up the exposure
							_Disparity.SetBrightness(BrightnessVal);
						}			
						else
						{
							 cout << endl << " Value out of Range - Invalid!!" << endl;
						}	
						break;
					}
				}
				BrightnessVal = -1;
				break;
			}
			
			if(BrightnessVal == -1)
			{ 			
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}							
		}
	}

	return 1;
}
 

