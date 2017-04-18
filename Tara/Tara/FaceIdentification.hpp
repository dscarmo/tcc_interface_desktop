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

#pragma once
#include "Tara.h"
#include "LocalBinaryPattern.hpp"


class FaceIdentification
{
public:
	
	//Constructor
	FaceIdentification(void);
	 
	//Initalises all the methods
	int Init();
			
private:
	//Image
	cv::Mat LeftImage;

	LocalBinaryPattern lbp;

	//Detects the faces in the scene
	//vector<Rect> DetectFace(Mat img);

	//Unscaling the point to the actual image size for the lower resolutions
	cv::Point unscalePoint(cv::Point Pt, cv::Size CurrentSize, cv::Size TargetSize);

	//Streams the input from the camera 
	int CameraStreaming(); 

	stringstream ss;  //To convert int to text
		
	//Object creation
	Tara::Disparity _Disparity;
};