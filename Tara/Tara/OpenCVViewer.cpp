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
	OpenCVViewer: Defines the methods to view the stereo images from 
				   the camera.
	
**********************************************************************/
#include "stdafx.h"
#include "OpenCVViewer.h"
#include <ctype.h>

//Initialises all the variables and methods
int OpenCVViewer::Init()
{	
	cout << endl  << "		OpenCV Viewer Application " << endl << " Displays the rectified left and right image!" << endl  << endl;

	//Init 
	if(!_Disparity.InitCamera(false, false)) //Initialise the camera
	{
		PrintDebug(DEBUG_ENABLED, L"Camera Initialisation Failed!");
		return 0;
	}
		
	//Streams the camera and process the height
	TaraViewer();

	return 1;
}

//Streams using OpenCV Application
//Converts the 10 bit data to 8 bit data and splits the left an d right image separately
int OpenCVViewer::TaraViewer()
{
	char WaitKeyStatus;
	Mat LeftImage, RightImage, FullImage;
	int BrightnessVal = 1;

	//Window Creation
	namedWindow("Input Image", WINDOW_NORMAL);
	
	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press t/T on the Image Window to change to Trigger Mode" << endl;
	cout << endl << "Press m/M on the Image Window to change to Master Mode" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Streams the Camera using the OpenCV Video Capture Object
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}

		//concatenate both the image as single image
		hconcat(LeftImage, RightImage, FullImage);

		imshow("Input Image", FullImage);

		//waits for the Key input
		WaitKeyStatus = waitKey(1);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			destroyAllWindows();
			break;
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
				cout << endl << "Selected mode and the current mode is the same!!" << endl;
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
				cout << endl << "Selected mode and the current mode is the same!!" << endl;
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

			while(getline(cin, Inputline)) //To avoid floats and Alphanumeric strings
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

//Main function 
int view_main()
{
	PrintDebug(DEBUG_ENABLED, L"OpenCV Camera Viewer");
	int ReturnStatus = -1;

	//Object to access the application to view the stereo images  
	OpenCVViewer _OpenCVViewer;

	//Initialises to run the viewer
	ReturnStatus = _OpenCVViewer.Init();
	
	PrintDebug(DEBUG_ENABLED, L"Exit: OpenCV Camera Viewer");
	cout << endl << "Exit: OpenCV Camera  Viewer" << endl << endl;
	
	if(!ReturnStatus) //check for a valid return
	{
		cout << endl << "Press any key to exit!" << endl << endl;
		_getch();
	}
	return 0;
}