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
	gray_xml_path = "xml\\gray_neural_" + personName + ".xml";
	depth_xml_path = "xml\\depth_neural_" + personName + ".xml";

	//Fill vector gray
	//Fill vector depth
	fillVector(gray, grayPath);
	fillVector(depth, depthPath);

	//Debug
	for (int i = 0; i < grayFaces.size(); i++) {
		imshow("gray faces", grayFaces[i]);
		waitKey(1);
	}
	for (int j = 0; j < depthFaces.size(); j++) {
		imshow("depthFaces", depthFaces[j]);
		waitKey(1);
	}

}

int Person::trainGrayNN(int neuronios) {
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

	Mat samples(Size(256, 27 + 27), CV_32F); //Positivos mais negativos
	Mat responses(Size(1, 27 + 27), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : grayFaces) {
		//this is confusing
		Mat trainingHist = test_lbp.grayLBPpipeline(positivo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	for (Mat negativo : depthFaces) { //Must get a negative dataset for faces
		//this is confusing
		Mat trainingHist = test_lbp.grayLBPpipeline(negativo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = -1;
		contador++;
	}

	if (!graynn->train(samples, ml::ROW_SAMPLE, responses))
		return CREATION_ERROR;

	//cout << "Treino concluído" << endl;
	graynn -> save(gray_xml_path);
	cout << "rede neural de " + personName + " treinada..." << endl;
	return TREINO_COMPLETO;
}

double Person::predict(Mat input) {
	Mat feature(Size(256, 1), CV_32F), output;
	Mat input_lbp = test_lbp.grayLBPpipeline(input);
	input_lbp.row(0).copyTo(feature.row(0));

	graynn->predict(feature, output);
	cout << output.at<float>(0, 0) << endl;
	return (double)output.at<float>(0, 0);
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
	people.push_back(new Person(1, "diedre", "Faces\\diedre\\gray", "Faces\\diedre\\depth"));
	people.push_back(new Person(2, "gin", "Faces\\gin\\gray", "Faces\\gin\\depth"));
	people.push_back(new Person(3, "raul", "Faces\\raul\\gray", "Faces\\raul\\depth"));
	people.push_back(new Person(4, "nicole", "Faces\\nicole\\gray", "Faces\\nicole\\depth"));
	people.push_back(new Person(5, "pompilio", "Faces\\pompilio\\gray", "Faces\\pompilio\\depth"));
	people.push_back(new Person(6, "rodolfo", "Faces\\rodolfo\\gray", "Faces\\rodolfo\\depth"));

	//Test
	people[0] -> trainGrayNN(5);
	for (Person* person : people) {
		cout << "predicting gray of " + person->personName + " in diedre gray network";
		for (Mat face : person->grayFaces)
			people[0]->predict(face);

		cout << "predicting depth of " + person->personName + " in diedre grau network";
		for (Mat face : person->depthFaces)
			people[0]->predict(face);
	}

}