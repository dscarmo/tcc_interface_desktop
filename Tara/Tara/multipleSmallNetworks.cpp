#include "stdafx.h"
#include <dirent.h>
#include "multipleSmallNetworks.hpp"
#include "Utils.hpp"

using namespace std;
using namespace cv;

Person::Person(int id, string name, const char* grayPath, const char * depthPath){
	//Put arguments and pushback faces
	trained = false;
	personID = id;
	personName = name;
	Mat readGray;
	Mat depthGray;
	gray_xml_path = "xml\\gray_neural_" + personName + ".xml";
	depth_xml_path = "xml\\depth_neural_" + personName + ".xml";
	negativos_path = "Faces\\negativos\\gray";
	validation_path = "Faces\\validation\\gray";

	//Fill vector gray
	//Fill vector depth
	if (id == 999) fillVector(validation, validation_path);
	else {
		fillVector(gray, grayPath);
		fillVector(depth, depthPath);
		fillVector(negativo, negativos_path);
	}

	//Debug
	for (int i = 0; i < grayFaces.size(); i++) {
		imshow("gray faces", grayFaces[i]);
		waitKey(1);
	}
	for (int j = 0; j < depthFaces.size(); j++) {
		imshow("depthFaces", depthFaces[j]);
		waitKey(1);
	}
	for (int j = 0; j < negativos.size(); j++) {
		imshow("negativos", negativos[j]);
		waitKey(1);
	}
	for (int j = 0; j < validation_imgs.size(); j++) {
		imshow("validation", validation_imgs[j]);
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
	graynn -> setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 0, 0);
	graynn -> setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	graynn -> setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);
	//graynn -> setBackpropWeightScale(0.1f);
	//graynn -> setBackpropMomentumScale(0.1f);

	Mat samples(Size(256, grayFaces.size() + negativos.size()), CV_32F); //Positivos mais negativos
	Mat responses(Size(1, grayFaces.size() + negativos.size()), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : grayFaces) {
		//this is confusing
		Rect roi(20, 20, positivo.size().width - 40, positivo.size().height - 40);
		//imshow("positivo sendo treinado", positivo(roi)); waitKey(0);
		Mat trainingHist = test_lbp.grayLBPpipeline(positivo(roi));
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	//take one face from each other
	for (Mat negativo : negativos) { //Must get a negative dataset for faces
		//this is confusing
		Rect roi(20, 20, negativo.size().width - 40, negativo.size().height - 40);

		Mat trainingHist = test_lbp.grayLBPpipeline(negativo(roi));
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = -1;
		contador++;
	}

	if (!graynn->train(samples, ml::ROW_SAMPLE, responses))
		return CREATION_ERROR;

	trained = true;
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
			Mat image;
			resize(imread(oss.str(), 0),image, Size(100,100));
			equalizeHist(image, image);
			if (!image.empty())
				cout << "Imagem lida com sucesso." << endl;
			else {
				cout << "Erro na leitura de imagem, continuando." << endl;
				continue;
			}
			if (gord == gray) grayFaces.push_back(image.clone());
			else if (gord == depth) depthFaces.push_back(image.clone());
			else if (gord == negativo) negativos.push_back(image.clone());
			else if (gord == validation) validation_imgs.push_back(image.clone());
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
	people.push_back(new Person(999, "validation", "Faces\\rodolfo\\gray", "Faces\\rodolfo\\depth"));

	//Test
	for (Person* person : people) {
		if (person->personName == "validation") continue;
		person->trainGrayNN(5);
	}

	/*for (Person* person : people) {
		cout << "predicting gray of " + person->personName + " in diedre gray network";
		int max = 0;
		for (Mat face : person->grayFaces) {
			Rect roi(20,20, face.size().width - 40, face.size().height - 40);
			people[0]->predict(face(roi));
			imshow("predicting isso aqui",face(roi));
			waitKey(0);

		}

	}*/

	for (Mat validation : people[6]->validation_imgs) {
		Rect roi(20, 20, 60, 60);
		validation = validation(roi);
		imshow("validation", validation);
		identificate(validation);
		waitKey(0);
	}
	cout << "para";



}

string MSN::identificate(cv::Mat input) {
	map<double, string> results;

	for (Person* person : people) 
	{	

		if (person->trained) 
		{
			double result = person->predict(input);
			string name = person->personName;
			//cout << person->personName + " saida da NN: " + to_string(result);
			results.insert(pair<double, string >(result, name));
		}
		else cout << person -> personName + " não treinado.";

		
		
	}
	for (auto r : results) {
		cout << r.first << r.second << endl << endl;
	}



	return "lol";

}