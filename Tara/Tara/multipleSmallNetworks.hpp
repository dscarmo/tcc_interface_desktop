#pragma once
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include "LocalBinaryPattern.hpp"

#define CREATION_ERROR 1
#define TREINO_COMPLETO 0
//personID
//personName
//gray faces
//depth faces
//gray cnn
//depth cnn

class Person {
private:
	enum grayOrDepth { gray, depth};
	std::string gray_xml_path;
	std::string depth_xml_path;
	cv::Ptr<cv::ml::ANN_MLP> graynn;
	cv::Ptr<cv::ml::ANN_MLP> depthnn;
	void fillVector(int gord, const char* path);
	//Test, will be in msn
	LocalBinaryPattern test_lbp;

public:

	int personID;
	bool trained;
	std::string personName;
	std::vector<cv::Mat> grayFaces;
	std::vector<cv::Mat> depthFaces;
	std::vector<cv::Mat> negativos;
	std::vector<cv::Mat> dnegativos;

	//constructor
	Person(int id, std::string name, const char* grayPath, const char* depthPath);
	
	//Train graynn on grayfaces
	int trainGrayNN(int neuronios);
	
	//Train depthnn on depthfaces
	int trainDepthNN(int neuronios);

	//Loads gray XML
	void loadGrayNN();

	//Load depth XML
	void loadDepthNN();

	//predict with input (runs both cnns predicts and weigth add returns result)
	double predict(cv::Mat input, cv::Mat dinput);

};

class MSN {
	//Dataset class (person) vector
private:
	std::vector<Person *> people;
public:
	//identificate (run all Persons predicts (parallel) sync and return better result
	//returns person name string
	std::string identificate(cv::Mat input, cv::Mat dinput);

	//constructor (hardcoded loop through all person folders creating Persons to add in vector
	MSN();
};

