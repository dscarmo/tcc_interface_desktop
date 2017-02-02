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
 DepthViewer: Displays the depth of the point selected by the user
			   using the disparity image computed.
**********************************************************************/
#include "stdafx.h"
#include "DepthViewer.h"

//Local point to access the user selected value
Point g_SelectedPoint(-1, -1);

//Initialises all the necessary files
int DepthViewer::Init()
{
	cout << endl << "		Depth Viewer Application " << endl  << endl;
	cout << " Depth Viewer - Displays the Filtered Disparity between the two frames" << endl << " Closer objects appear in Red and Farther objects appear in Blue Color!"<< endl;
	cout << " Select a point to display the depth of the point!" << endl  << endl;

	//MY STUFF HERE

	//Initialise the Camera
	if(!_Disparity.InitCamera(true, true))
	{
		PrintDebug(DEBUG_ENABLED, L"Initialisation Failed");
		return 0;
	}
	
	//Camera Streaming
	CameraStreaming();

	return 1;
}

//Streams the input from the camera
int DepthViewer::CameraStreaming()
{	
	float DepthValue = 0;
	char WaitKeyStatus;
	bool GrayScaleDisplay = false;
	Mat LeftImage, RightImage;
	Mat gDisparityMap, gDisparityMap_viz;
	int BrightnessVal = 4;		//Default value

	//Window Creation
	namedWindow("Disparity Map", WINDOW_AUTOSIZE);
	namedWindow("Left Image", WINDOW_AUTOSIZE);
	namedWindow("Right Image", WINDOW_AUTOSIZE);

	//Mouse callback set to disparity window
    setMouseCallback("Disparity Map", DepthPointSelection);

	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press t/T on the Image Window to change to Trigger Mode" << endl;
	cout << endl << "Press m/M on the Image Window to change to Master Mode" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl;
	cout << endl << "Press d/D on the Image Window to view the grayscale disparity map!" << endl << endl;

	//cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Dispalys the filtered disparity, the depth of the point selected is displayed
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}
	
		//Get disparity
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);

		//Estimate the Depth of the point selected
		_Disparity.EstimateDepth(g_SelectedPoint, &DepthValue);

		if(g_SelectedPoint.x > -1 && DepthValue > 0) //Mark the point selected by the user
			circle(gDisparityMap_viz, g_SelectedPoint, 3, Scalar::all(0), 3, 8);

		if(DepthValue > 0)
		{
			stringstream ss;
			ss << DepthValue / 10 << " cm\0" ;
			DisplayText(gDisparityMap_viz, ss.str(), g_SelectedPoint);
		}

		//Display the Images
		imshow("Disparity Map", gDisparityMap_viz);		
		imshow("Left Image",  LeftImage);		
		imshow("Right Image", RightImage);

		if(GrayScaleDisplay)
		{			
			imshow("Disparity Map GrayScale", gDisparityMap);						
		}

		//waits for the Key input
		WaitKeyStatus = waitKey(1);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			destroyAllWindows();
			break;
		}		
		else if(WaitKeyStatus == 'd' || WaitKeyStatus == 'D')
		{	
			if(!GrayScaleDisplay)
			{
				GrayScaleDisplay = true;
				namedWindow("Disparity Map GrayScale", WINDOW_AUTOSIZE);
			}
			else 
			{
				GrayScaleDisplay = false;
				destroyWindow("Disparity Map GrayScale");
			}
			
		}
		//Sets up the mode
		else if(WaitKeyStatus == 'T' || WaitKeyStatus == 't' ) //Stream Mode 0 - Trigger Mode 1 - Master Mode
		{			
			if(_Disparity.SetStreamMode(TRIGGERMODE))
			{
				cout << endl << "Switching to Trigger Mode!!" << endl;
			}
			else
			{
				cout << endl << "Selected mode and the current mode is the same!" << endl;
			}
		}

		//Sets up the mode
		else if(WaitKeyStatus == 'M' || WaitKeyStatus == 'm' ) //Stream Mode 0 - Trigger Mode 1 - Master Mode
		{			
			if(_Disparity.SetStreamMode(MASTERMODE))
			{
				cout << endl << "Switching to Manual Mode!!" << endl;
			}
			else
			{
				cout << endl << "Selected mode and the current mode is the same!" << endl;
			}
		}		
		//Sets up Auto Exposure
		else if(WaitKeyStatus == 'a' || WaitKeyStatus == 'A' ) //Auto Exposure
		{
			UINT StreamMode = -1;
			_Disparity.GetStreamMode(&StreamMode);

			if(StreamMode == TRIGGERMODE)
			{
				cout << endl << "Switch to Master Mode to set Auto Exposure!!" << endl;
			}
			else
			{
				int CurrentExpValue = 0;
				GetManualExposureValue_Stereo(&CurrentExpValue);
				if(CurrentExpValue != 1)
				{
					cout << endl << "Switching to Auto Exposure!!" << endl;
					//Setting up the exposure
					_Disparity.SetExposure(SEE3CAM_STEREO_AUTO_EXPOSURE);
				}
				else
				{
					cout << endl << "Already in Auto Exposure!!" << endl;
				}
			}
		}

		else if(WaitKeyStatus == 'e' || WaitKeyStatus == 'E') //Set Exposure
		{		
			cout << endl << "Enter the Exposure Value Range(10 to 1000000 micro seconds): " << endl;
			
			ManualExposure = 0;			
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats and Alphanumeric strings
			{			
				std::stringstream ss(Inputline);
				if (ss >> ManualExposure)
				{					
					if (ss.eof())
					{  						
						if(ManualExposure >= SEE3CAM_STEREO_EXPOSURE_MIN && ManualExposure <= SEE3CAM_STEREO_EXPOSURE_MAX)
						{ 							
							//Setting up the exposure
							_Disparity.SetExposure(ManualExposure);	
						}
						else
						{							
							cout << endl << " Value out of Range - Invalid!!" << endl;							
						}	
						break;
					}
				}
				ManualExposure = -1;
				break;
			}
			
			if(ManualExposure == -1)
			{ 				
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}			
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

//Call back function
void DepthPointSelection(int MouseEvent, int x, int y, int flags, void* param) 
{
    if(MouseEvent == CV_EVENT_LBUTTONDOWN)  //Clicked
	{
		g_SelectedPoint = Point(x, y);
    }
}

//Main Application
int main()
{
	PrintDebug(DEBUG_ENABLED, L"Depth Viewer");
	int ReturnStatus = -1;

	//Object creation
	DepthViewer _DepthViewer;

	//Initialises the depth view
	ReturnStatus = _DepthViewer.Init();

	PrintDebug(DEBUG_ENABLED, L"Exit: Depth Viewer");
	cout << endl << "Exit: Depth Viewer" << endl << endl;
	
	if(!ReturnStatus) //check for a valid return
	{
		cout << endl << "Press any key to exit!" << endl << endl;
		_getch();
	}
	return 0;
}
