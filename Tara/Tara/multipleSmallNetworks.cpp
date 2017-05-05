#include "stdafx.h"
#include <dirent.h>
#include "multipleSmallNetworks.hpp"
#include "Utils.hpp"

using namespace std;
using namespace cv;

Person::Person(int id, string name, const char* grayPath, const char * depthPath){
	//Put arguments and pushback faces
	personID = id;
	personName = name;
	Mat readGray;
	Mat depthGray;

	//Fill vector gray
	//Fill vector depth
	fillVector(gray, grayPath);
	fillVector(depth, depthPath);

	//Debug
	for (int i = 0; i < grayFaces.size(); i++) {
		imshow("gray faces", grayFaces[i]);
		waitKey(100);
	}
	for (int j = 0; j < depthFaces.size(); j++) {
		imshow("depthFaces", depthFaces[j]);
		waitKey(100);
	}

}

void Person::trainGrayNN() {
	int neuronios = 5;
	Mat layers = Mat(3, 1, CV_32S);
	layers.row(0) = Scalar(256);
	layers.row(1) = Scalar(neuronios);
	layers.row(2) = Scalar(1);
	//neural
	graynn = ml::ANN_MLP::create();
	graynn -> setLayerSizes(layers);
	graynn -> setTrainMethod(ml::ANN_MLP::BACKPROP);
	graynn -> setTermCriteria(TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 3000, 0.000001f));
	graynn -> setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
	graynn -> setBackpropWeightScale(0.1f);
	graynn -> setBackpropMomentumScale(0.1f);

	//Positivos
	for (Mat positivo : grayFaces) {
		//this is confusing
	}
}

void Person::fillVector(int gord, const char* input_dir) {
	vector<String> all_files;

	//Onde ta as imagem

	//const char* input_dir = "Faces";
	//const char* input_dir = "Faces\\diedre_night";
	//const char* input_dir = "Faces\\";

	//Pointer to input directory
	DIR *dir_ptr = opendir(input_dir);
	if (dir_ptr == NULL) {
		cout << "DIR NULL POINTER!!! Insert correct hardcoded dataset directory" << endl;
		system("PAUSE");
	}

	while (true) {
		dirent *dir_ent = readdir(dir_ptr);

		if (NULL == dir_ent) {
			break;
		}

		all_files.push_back(string(dir_ent->d_name));
	}
	closedir(dir_ptr);

	//Sort file names 
	sort(all_files.begin(), all_files.end());


	cout << input_dir << " diretorio sendo processado" << endl;

	vector<String> image_files;
	for (String file : all_files) {
		if (ends_with(".png", file) || ends_with(".jpg", file) || ends_with(".jpeg", file)) {
			ostringstream oss;
			oss << input_dir << "\\" << file;
			Mat image = imread(oss.str());
			if (!image.empty())
				cout << "Imagem lida com sucesso." << endl;
			else {
				cout << "Erro na leitura de imagem, continuando." << endl;
				continue;
			}
			if (gord == gray) grayFaces.push_back(image.clone());
			else if (gord == depth) depthFaces.push_back(image.clone());
		}
	}


}

MSN::MSN() {
	people.push_back(Person(1, "diedre", "Faces\\diedre\\gray", "Faces\\diedre\\depth"));
}