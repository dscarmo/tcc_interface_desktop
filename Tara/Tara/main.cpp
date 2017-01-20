#include<iostream>
#include "stdafx.h"
#include "OpenCVViewer.h"
#include "DepthViewer.h"

using namespace std;

int main() {
	printf("OpenCV: %s", cv::getBuildInformation().c_str());
	int opt;
	system("PAUSE");
	cout << "view(0) ou depth(1)?" << endl;
	cin >> opt;
	if (opt) 
	{
		depth_main();
	}
	else 
	{
		view_main();
	}
}