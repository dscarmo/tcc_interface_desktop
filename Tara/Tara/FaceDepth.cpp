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
	FaceDepth: Estimates the depth of the person from the camera by 
			    detecting the face of the person
**********************************************************************/
#include "stdafx.h"
#include "FaceDepth.h"
#include "FaceDetection.hpp"
#define RIGHTMATCH 150 //disparity range starts from 150 so in case of the point being less than 150 it means that the face is not fully covered in the right.

//Constructor
FaceDepth::FaceDepth(void)
{
	ss.precision(2); //Restricting the precision after decimal
}

//Initialises all the variables and methods
int FaceDepth::Init()
{	
	cout << endl << "		Face Detection Application " << endl  << endl;
	cout << " Detects multiple faces in the frame!" << endl << " Uses OpenCV Haarcascade file in the Face folder!" << endl;
	cout << " Rectangle around the detected faces with the depth is displayed!" << endl << " Only the left window is displayed!" << endl << endl;

	//Loads the cascade file
	//FaceCascade.load("haarcascade_frontalface_alt.xml");
	FaceCascade.load("lbpcascade_frontalface.xml");

	//If the file is not loaded properly
	if(FaceCascade.empty())
	{		
		PrintDebug(DEBUG_ENABLED, L"Haarcascade Classifier File Missing!");	
		printf("\n Haarcascade Classifier File Missing!\n");	
		return 0;
	}
	
	PrintDebug(DEBUG_ENABLED, L"Loaded Haarcascade Classifier File!");	

	if(!_Disparity.InitCamera(true, true)) //Initialise the camera
	{
		PrintDebug(DEBUG_ENABLED, L"Camera Initialisation Failed!");
		return 0;
	}

	//Streams the camera and process the height
	CameraStreaming();

	return 1;
}

//Unscaling the point to the actual image size for the lower resolutions
Point FaceDepth::unscalePoint(Point Pt, Size CurrentSize, Size TargetSize)
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
int FaceDepth::CameraStreaming()
{	
	stringstream ss;
	vector<Rect> LFaces;
	float DepthValue;
	bool RImageDisplay = false;
	int BrightnessVal = 4;		//Default value
	Mat gDisparityMap, gDisparityMap_viz, RightImage, DisplayImage, depthMap;
	Point g_SelectedPoint;
	
	//user key input
	char WaitKeyStatus;

	//Window Creation
	namedWindow("Left Image", WINDOW_AUTOSIZE);
	
	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" <<endl;
	cout << endl << "Press r/R on the Image Window to see the right image" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;
	bool save = false;
	int saved = 0;


	//Estimates the depth of the face using Haarcascade file of OpenCV
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}	
	
		//Get disparity
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);
		
		//Detect the faces
		try
		{
			LFaces = faceDetect(LeftImage);
		}
		catch (const std::exception&)
		{
				
		}
		
		//Construct 
		int SCALE = 10;
		FileStorage testFile("testSave.xml", FileStorage::WRITE);
		depthMap = Mat::zeros(gDisparityMap.size(), CV_16UC1);

		//Estimate the Depth of the point selected and save/show 3D face
		for (size_t i = 0; i < LFaces.size(); i++)
		{
			//Pointing to the center of the face
			g_SelectedPoint = Point(LFaces[i].x + LFaces[i].width / 2, LFaces[i].y + LFaces[i].height / 2);

			Point scaledPoint = unscalePoint(g_SelectedPoint, LeftImage.size(),  gDisparityMap.size());
			
			//Find the depth of the point passed
			_Disparity.EstimateDepth(scaledPoint, &DepthValue);
			
			if(g_SelectedPoint.x > RIGHTMATCH && DepthValue > 0) //Mark the point selected by the user
			{				
				ss << DepthValue / (10 * 2) << " cm\0" ;
				DisplayText(LeftImage, ss.str(), Point(LFaces[i].x, LFaces[i].y));
				ss.str(string());
			}

			faceRectangle = gDisparityMap(LFaces[i]);
			imshow("retanguloFace", faceRectangle);
			
			if (save) {
				save = false;
				imwrite("Faces\\" + to_string(saved++) + ".jpg", LeftImage(LFaces[i]));
				imwrite("Faces\\" + to_string(saved++) + "disp.jpg", gDisparityMap(LFaces[i]));
				testFile << "test_disp" << gDisparityMap(LFaces[i]);
			}
		}

		//imshow("depth", depthMap);
		
		//Display the Images
		DisplayImage = drawFps(LeftImage);
		imshow("Left Image",  DisplayImage);			

		//Mouse and keyboard callbacks
		setMouseCallback("retanguloFace", onMouse, 0);

		//waits for the Key input
		WaitKeyStatus = waitKey(10);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			vector<Rect>().swap(LFaces);
			destroyAllWindows();
			testFile.release();
			break;
		}

		else if (WaitKeyStatus == 's' || WaitKeyStatus == 'S') //Show the right image
		{
			cout << "Saving next detected face" << endl;
			save = true;
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
 

//Detects the faces in the image passed
//
//vector<Rect> FaceDepth::DetectFace(Mat InputImage)
//{		
//	vector<Rect> FacesDetected;
//
//	//Detects the faces in the scene
//	FaceCascade.detectMultiScale(InputImage, FacesDetected/*, 1.3, 5*/);
//
//	//Marks the faces on the image
//	for(size_t i = 0; i < FacesDetected.size(); i++)
//	{
//		rectangle(InputImage, FacesDetected[i], Scalar(255, 0, 0), 2);  
//	}
//
//	//Return the detected faces
//	return FacesDetected;
//}

//Main Function
/*
int main()
{
	int ReturnStatus = -1;
	PrintDebug(DEBUG_ENABLED, L"Face Detection!");

	//Object creation
	FaceDepth _FaceDepth;
	
	//Initialises the Face Detection Project
	ReturnStatus = _FaceDepth.Init();

	PrintDebug(DEBUG_ENABLED, L"Exit: Face Detection!");
	cout << endl << "Exit: Face Detection Application" << endl << endl;	
	
	if(!ReturnStatus) //check for a valid return
	{
		cout << endl << "Press any key to exit!" << endl << endl;
		_getch();
	}
	return 0;
}*/
