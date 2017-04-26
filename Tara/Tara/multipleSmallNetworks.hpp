#pragma once
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"

//personID
//personName
//gray faces
//depth faces
//gray cnn
//depth cnn

class Person {
private:
	int personID;
	std::string personName;
	std::vector<cv::Mat> grayFaces;
	std::vector<cv::Mat> depthFaces;
	cv::Ptr<ml::ANN_MLP> graynn;
	cv::Ptr<ml::ANN_MLP> depthnn;
	void fillVector(std::vector<cv::Mat> faces, const char* path);

public:
	//constructor
	Person(int id, std::string name, const char* grayPath, const char* depthPath);
	
	//Train graynn on grayfaces
	void trainGrayNN();
	
	//Train depthnn on depthfaces
	void trainDepthNN();

	//Loads gray XML
	void loadGrayNN();

	//Load depth XML
	void loadDepthNN();

	//predict with input (runs both cnns predicts and weigth add returns result)
	double predict(Mat input);

	//Trains local grayNN in gray images
	void trainGray();

	//Trains local depthNN in depth images 
	void trainDepth();


};

class MSN {
	//Dataset class (person) vector
private:
	std::vector<Person> people;
public:
	//identificate (run all Persons predicts (parallel) sync and return better result
	//returns person name string
	std::string identificate();

	//constructor (hardcoded loop through all person folders creating Persons to add in vector
	MSN();
};

