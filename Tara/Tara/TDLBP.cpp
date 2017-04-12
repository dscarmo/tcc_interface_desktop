//#include "boost/filesystem.hpp"

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/detection_based_tracker.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <math.h>
#include "stdafx.h"
#include "TDLBP.h"

using namespace std;
//using namespace boost::filesystem;
using namespace cv;
using namespace cv::ml;

int dec2bin(int number){
	// Converts decimal number to binary

	int bin = 0;
	int i = 1;


	while (number > 0){
		bin += (number % 2) * i;
		number = number / 2;
		i *= 10;
	}

	return bin;
}

void histogramCount(Mat &LBP, vector<int>& histogram, int x1, int x2, int y1, int y2){
	// Gets the histogram of the LBP values of a region of the image.

	int bins = 14;
	double division = (double) 256 / bins; // 0 - 255 possible intensity values;
	int temp; // debugging purposes

	for (int i = x1; i <= x2; i = i + 1){
		for (int j = y1; j <= y2; j++){

			
			temp = (float) floor((LBP.at<float>(i, j)) / division);
			histogram[temp] = histogram[temp] + 1;
		}
	}

}

int bin2dec(int number){
	// Converts binary to decimal

	int dec = 0;
	int rem;
	int i = 1;
	while (number > 0){
		rem = number % 10;
		dec = dec + rem * i;
		i = i * 2;
		number = number / 10;
	}

	return dec;
}




// Calculates the 3DLBP in the depth image. Remember that the image should be between 0 and 255 pixel intensitys.
Mat calculate3DLBP(Mat depth_image){

	// Initialize Matrices that will store the 4 LBP code numbers.
	Mat LBP1(depth_image.rows, depth_image.cols, CV_32F);
	Mat LBP2(depth_image.rows, depth_image.cols, CV_32F);
	Mat LBP3(depth_image.rows, depth_image.cols, CV_32F);
	Mat LBP4(depth_image.rows, depth_image.cols, CV_32F);

	// LBP Indexes to be visited
	vector<int> index_x, index_y;
	index_x.reserve(8);
	index_y.reserve(8);

	int vx [] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int vy [] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	index_x.insert(index_x.begin(), &vx[0], &vx[8]);
	index_y.insert(index_y.begin(), &vy[0], &vy[8]);

	// 8 Neighbors values
	vector<double> neigh;
	neigh.reserve(8);

	// Values of Neighbor - central pixel
	vector<double> subtraction;
	subtraction.reserve(8);

	//  LBP values
	int i1 = 0;
	int i2 = 0;
	int i3 = 0;
	int i4 = 0;

	// Variables needed
	double subtract;
	int temporary_bin = 0;

	// For all pixels in image performs 3DLBP
	for (int i = 0; i < depth_image.rows; i++){
		for (int j = 0; j < depth_image.cols; j++){

			// 8 Neighbors visited
			for (int k = 0; k < 8; k++){
				// Tests if goes out of image borders
				if (i + index_x[k] < 0 || i + index_x[k] >= depth_image.rows || j + index_y[k] < 0 || j + index_y[k] >= depth_image.cols){
					neigh.push_back(0);
				}
				else{
					neigh.push_back((double) depth_image.at<uchar>(i + index_x[k], j + index_y[k]));
				}
				
				subtract = (neigh[k] - (double) depth_image.at<uchar>(i,j));

				if (subtract > 7){
					subtraction.push_back(7);
				}

				else if (subtract < -7){
					subtraction.push_back(-7);
				}

				else{
					subtraction.push_back(subtract);
				}

				int i11 = (subtraction[k] >= 0);

				i1 = i1 + (i11 * pow(10, static_cast<double>(7 - k)));

				temporary_bin = dec2bin(int(abs(subtraction[k])));

				int i22 = temporary_bin / 100 % 10;
				int i33 = temporary_bin / 10 % 10;
				int i44 = temporary_bin / 1 % 10;

				i2 = i2 + (i22 * pow(10, static_cast<double>(7 - k)));
				i3 = i3 + (i33 * pow(10, static_cast<double>(7 - k)));
				i4 = i4 + (i44 * pow(10, static_cast<double>(7 - k)));

			}

			i1 = bin2dec(i1);
			i2 = bin2dec(i2);
			i3 = bin2dec(i3);
			i4 = bin2dec(i4);

			subtraction.clear();
			subtraction.reserve(8);
			neigh.clear();
			neigh.reserve(8);

			LBP1.at<float>(i, j) = (float) i1;
			LBP2.at<float>(i, j) = (float) i2;
			LBP3.at<float>(i, j) = (float) i3;
			LBP4.at<float>(i, j) = (float) i4;

			// Reset Parameters;
			i1 = 0;
			i2 = 0;
			i3 = 0;
			i4 = 0;
		}
	}

	// Now we compile all info in a histogram

	// Image will be divided in 8 x 8 total blocks (64 blocks)
	int numWindows_x = 8;
	int numWindows_y = 8;

	// Adapt the size of the blocks to the depth image size.
	int step_x = floor(static_cast<double>(depth_image.cols / numWindows_x));
	int step_y = floor(static_cast<double>(depth_image.rows / numWindows_y));

	// Variable needed for histogram count
	int count = 0;


	// Histograms will have 14 bins.
	vector<int> histogram(14, 0);
	vector<int> histogram2(14, 0);
	vector<int> histogram3(14, 0);
	vector<int> histogram4(14, 0);

	// Correspond to the end and start of the blocks
	int x1, x2, y1, y2;

	// 64 blocks each one having 14 bins. They will all be concatenated in the Final Descriptor
	Mat FinalDescriptor = Mat::zeros(Size(14 * 64 * 4, 1),CV_32FC1); 

	for (int n = 0; n < numWindows_y; n++){
		for (int m = 0; m < numWindows_x; m++){

			// Calculating start and end of the evaluated regions
			x1 = n * step_y;
			x2 = (n + 1)*step_y - 1;

			y1 = m * step_x;
			y2 = (m + 1)*step_x - 1;

			// Histogram count and Fill in the Final Frescriptor for the 4 LBP codes
			histogramCount(LBP1, histogram, x1, x2, y1, y2);
			for (int o = 0; o < 14; o++){
				FinalDescriptor.at<float>(count) = histogram[o];
				histogram[o] = 0;
				count++;
			}


			histogramCount(LBP2, histogram2, x1, x2, y1, y2);
			for (int o = 14; o < 28; o++){
				FinalDescriptor.at<float>(count) = histogram2[o - 14];
				histogram2[o - 14] = 0;
				count++;
			}


			histogramCount(LBP3, histogram3, x1, x2, y1, y2);
			for (int o = 28; o < 42; o++){
				FinalDescriptor.at<float>(count) = histogram3[o - 28];
				histogram3[o - 28] = 0;
				count++;
			}


			histogramCount(LBP4, histogram4, x1, x2, y1, y2);
			for (int o = 42; o < 56; o++){
				FinalDescriptor.at<float>(count) = histogram4[o - 42];
				histogram4[o - 42] = 0;
				count++;
			}

		}
	}

	return FinalDescriptor;

}

// ---------------------------------------- Main from guy -------------------------------------//

// Outputs a vector of strings with all the files in a path.
/*vector<string> listAllFilesInDirectory(path filePath){

	vector<string> files;

	directory_iterator end_itr;

	// cycle through the directory
	for (directory_iterator itr(filePath); itr != end_itr; ++itr)
	{
		// If it's not a directory, list it. If you want to list directories too, just remove this check.
		if (is_regular_file(itr->path())) {
			// assign current file name to current_file and echo it out to the console.
			string current_file = itr->path().string();
			files.push_back(current_file);
		}
	}

	return files;
}


// Adapted for 12 test images and 52 identitys, if needed Change this.
float getClassifierAccuracy(Mat predicted, Mat groundTruth){
	int correct_predictions = 0;
	for (int i = 0; i < 52 * 12; i++){
		if ((int) predicted.at<float>(i) == groundTruth.at<int>(i))
		{
			correct_predictions++;
		}
			
	}
	
	float accuracy = (float) correct_predictions / (52 * 12);
	return accuracy;
}

float getClassifierAccuracy_forLO(Mat predicted, Mat groundTruth){
	int correct_predictions = 0;
	for (int i = 0; i < 52 * 12-6; i=i+6){
		if ((int) predicted.at<float>(i) == groundTruth.at<int>(i))
		{
			correct_predictions++;
		}

	}

	float accuracy = (float) correct_predictions / (52 * 2);
	return accuracy;
}


int main_3dlbp(){

	// Get all images available in the dataset path.
	path DatabaseFilePath = "E:/Thesis Datasets/EURECOM - Preprocessed/KinectDB Normalize/";
	vector<string> files = listAllFilesInDirectory(DatabaseFilePath);

	// Variables initialization
	Mat image;
	
	// Each image will have a feature vector of 8x8x4x14 features. We need memory allocation due to the array size.
	// Feature Vector Size: 14 bins x (8x8) regions x 4 LBP codes = 3584 feature vector
	// In preliminary dataset -> 5 subjects. 6 Samples - > 2 for testing, 2 training
	Mat trainingDataMat(52 * 2, 14 * 8 * 8 * 4, CV_32F);
	Mat labelsMat(52 * 2, 1, CV_32S);
	Mat testDataMat(52 * 12, 14 * 8 * 8 * 4, CV_32F);
//…
	Mat labelsTest(52 * 12, 1, CV_32S);
	string current_filename;
	
	cout << "Prepare for feature extraction ... " << endl << endl; 

	int training_samples_visited = 0;
	int test_samples_visited = 0;

	Mat descriptor(14 * 8 * 8 * 4, 1, CV_32F);

	for (int i = 0; i < files.size(); i++){
		current_filename = files[i].substr(files[i].size() - 6, 2);
		if (current_filename != "al"){
			// If image isn't neutral save in testDataMat
			image = imread(files[i], 0);
			cv::Mat sel = cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5));
			morphologyEx(image, image, MORPH_CLOSE, sel);
//…
		


			labelsTest.at<int>(test_samples_visited) = (int) test_samples_visited / 12 + 1;
			
			// Get feature vector
			descriptor = calculate3DLBP(image);

			// Save feature vector in the trainingData
			for (int j = 0; j < descriptor.cols; j++){
				testDataMat.at<float>(test_samples_visited, j) = descriptor.at<float>(j);
			}

			test_samples_visited++;
		}
		else{
			// If image is neutral save in trainingDataMat
			image = imread(files[i], 0);
			cv::Mat sel = cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5));
			morphologyEx(image, image, MORPH_CLOSE, sel);


			labelsMat.at<int>(training_samples_visited) = (int) training_samples_visited / 2 + 1;
			
			// Get feature vector
			descriptor = calculate3DLBP(image);

			// Save feature vector in the trainingData
			for (int j = 0; j < descriptor.cols; j++){
				trainingDataMat.at<float>(training_samples_visited, j) = descriptor.at<float>(j);
			}

			training_samples_visited++;
		}


		cout << "Feature Extraction of image " << i + 1 << " of a total of " << 52 * 14 << endl;
	}

	cout << endl <<  "Feature Extraction ended successfully!" << endl << endl;

	// Prepare the SVM
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::RBF);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

	float best_accuracy = 0;
	float best_Gamma = 0;
	float best_C = 0;
	float current_C, current_gamma;
	float current_accuracy;

	cout << "Starting dense-grid parameter estimation for C-SVM, using a RBF kernel... " << endl << endl;
	
	int current_model = 1;

	Mat predictedLabels(52 * 12, 1, CV_32S);
	
	// Dense grid between 10^-15 and 10^15 for gamma and C
	for (int i = -15; i < 16; i++){
		for (int j = -15; j < 16; j++){
			current_C = (float) pow(10, i);
			current_gamma = (float) pow(10, j);
			svm->setC(current_C);
			svm->setGamma(current_gamma);
			
			

			// Train Model
			svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
			svm->predict(testDataMat, predictedLabels);						

			current_accuracy = getClassifierAccuracy(predictedLabels, labelsTest) * 100;
			float LO = getClassifierAccuracy_forLO(predictedLabels, labelsTest) * 100;


			cout << "Training model " << current_model << " of " << 31 * 31 << ". (C = " << current_C << " , G = " <<
				current_gamma << ") - Accuraccy: " << current_accuracy << " %. (LO  = " << LO << " %)" << endl;

			if (current_accuracy >= best_accuracy){
				best_C = current_C;
				best_Gamma = current_gamma;
				best_accuracy = current_accuracy;
			}

			current_model++;
		}
	}

	cout << "Dense Grid estimation ended! " << endl << endl;

	cout << "Best C: " << best_C << endl;
	cout << "Best Gamma: " << best_Gamma << endl;
	cout << "Best Accuracy: " << best_accuracy << " %" << endl << endl;
	
	// Train and save best model.
	svm->setC(best_C);
	svm->setGamma(best_Gamma);
	svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
	svm->save("Best_Model_3DLBP.xml");

	system("Pause");
	return 0;
}*/

