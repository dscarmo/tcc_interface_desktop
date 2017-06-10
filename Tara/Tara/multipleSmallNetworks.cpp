#include "stdafx.h"
#include <dirent.h>
#include "multipleSmallNetworks.hpp"
#include "Utils.hpp"
#include "TDLBP.h"

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

	//Fill vector gray
	//Fill vector depth
	fillVector(gray, grayPath);
	fillVector(depth, depthPath);

	//Debug
	/*for (int i = 0; i < grayFaces.size(); i++) {
		imshow("gray faces", grayFaces[i]);
		waitKey(1);
	}
	for (int j = 0; j < depthFaces.size(); j++) {
		imshow("depthFaces", depthFaces[j]);
		waitKey(1);
	}
	for (int j = 0; j < validation_imgs.size(); j++) {
		imshow("validation", validation_imgs[j]);
		waitKey(1);
	}*/

}

int Person::trainDepthNN(int neuronios) {
	Mat layers = Mat(3, 1, CV_32S);
	layers.row(0) = Scalar(3584);
	layers.row(1) = Scalar(neuronios);
	layers.row(2) = Scalar(1);
	//neural
	depthnn = ml::ANN_MLP::create();
	depthnn->setLayerSizes(layers);
	depthnn->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 0, 0);
	depthnn->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	depthnn->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);
	//graynn -> setBackpropWeightScale(0.1f);
	//graynn -> setBackpropMomentumScale(0.1f);

	Mat samples(Size(3584, depthFaces.size() + negativos.size()), CV_32F); //Positivos mais negativos
	Mat responses(Size(1, depthFaces.size() + negativos.size()), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : depthFaces) 
	{
		Rect roi(20, 20, positivo.size().width - 40, positivo.size().height - 40);
		calculate3DLBP(positivo).row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	//take one face from each other
	for (Mat negativo : dnegativos) 
	{ 
		Rect roi(20, 20, negativo.size().width - 40, negativo.size().height - 40);
		calculate3DLBP(negativo).row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = -1;
		contador++;
	}

	if (!depthnn->train(samples, ml::ROW_SAMPLE, responses))
		return CREATION_ERROR;

	trained = true;
	//cout << "Treino concluído" << endl;
	depthnn->save(depth_xml_path);
	cout << "rede neural depth de " + personName + " treinada..." << endl;
	return TREINO_COMPLETO;
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

	Mat samples(Size(256, grayFaces.size() + negativos.size()), CV_32F); //Positivos mais negativos
	Mat responses(Size(1, grayFaces.size() + negativos.size()), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : grayFaces) {
		//this is confusing
		//Rect roi(20, 20, positivo.size().width - 40, positivo.size().height - 40);
		//imshow("positivo sendo treinado", positivo(roi)); waitKey(0);
		//Mat trainingHist = test_lbp.grayLBPpipeline(positivo(roi));
		Mat trainingHist = test_lbp.grayLBPpipeline(positivo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	//take one face from each other
	for (Mat negativo : negativos) { //Must get a negative dataset for faces
		//this is confusing
		//Rect roi(20, 20, negativo.size().width - 40, negativo.size().height - 40);

		//Mat trainingHist = test_lbp.grayLBPpipeline(negativo(roi));

		Mat trainingHist = test_lbp.grayLBPpipeline(negativo);
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

double Person::predict(Mat input, Mat dinput) {
	Mat feature(Size(256, 1), CV_32F), output, doutput;
	Mat dfeature(Size(3584, 1), CV_32F);
	Mat input_lbp = test_lbp.grayLBPpipeline(input);
	input_lbp.row(0).copyTo(feature.row(0));
	double points2d, points3d;
	points2d = 0;
	points3d = 0;
	int PESODEPTH = 0;
	int PESO = 1;

	//Gray
	graynn->predict(feature, output);
	points2d = (double)PESO*output.at<float>(0, 0);
	cout << "Gray de " + personName + ": " + to_string(points2d/PESO) << endl;
	
	//Depth
	if (!dinput.empty())
	{
		imshow("depth feature", dinput);
		dfeature = calculate3DLBP(dinput);
		depthnn->predict(dfeature, doutput);
		points3d = PESODEPTH*doutput.at<float>(0, 0);
		cout << "Depth de " + personName + ": " + to_string(points3d/PESODEPTH) << endl;
	}

	return points2d + points3d;
}

void Person::fillVector(int gord, const char* input_dir) {
	vector<String> all_files;

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
			if (gord == gray) equalizeHist(image, image);
			if (!image.empty())
				cout << "Imagem lida com sucesso." << endl;
			else 
			{
				cout << "Erro na leitura de imagem, continuando." << endl;
				continue;
			}
			if (gord == gray) grayFaces.push_back(image.clone());
			else if (gord == depth) depthFaces.push_back(image.clone());
		}
	}


}

MSN::MSN() {

	//Yale
	people.push_back(new Person(1, "asian_woman", "Faces\\asian_woman\\gray", "Faces\\asian_woman\\depth"));
	people.push_back(new Person(2, "big_eye", "Faces\\big_eye\\gray", "Faces\\big_eye\\depth"));
	people.push_back(new Person(3, "bigode_limpo", "Faces\\bigode_limpo\\gray", "Faces\\bigode_limpo\\depth"));
	people.push_back(new Person(4, "bigode_marcado", "Faces\\bigode_marcado\\gray", "Faces\\bigode_marcado\\depth"));
	people.push_back(new Person(5, "sobrancelha_asia", "Faces\\sobrancelha_asia\\gray", "Faces\\sobrancelha_asia\\depth"));
	people.push_back(new Person(999, "validation", "Faces\\validation\\gray", "Faces\\validation\\depth"));

	//My dataset
	/*
	people.push_back(new Person(1, "diedre", "Faces\\diedre\\gray", "Faces\\diedre\\depth"));
	people.push_back(new Person(2, "gin", "Faces\\gin\\gray", "Faces\\gin\\depth"));
	people.push_back(new Person(3, "raul", "Faces\\raul\\gray", "Faces\\raul\\depth"));
	people.push_back(new Person(4, "nicole", "Faces\\nicole\\gray", "Faces\\nicole\\depth"));
	people.push_back(new Person(5, "pompilio", "Faces\\pompilio\\gray", "Faces\\pompilio\\depth"));
	people.push_back(new Person(6, "rodolfo", "Faces\\rodolfo\\gray", "Faces\\rodolfo\\depth"));
	people.push_back(new Person(7, "darline", "Faces\\darline\\gray", "Faces\\darline\\depth"));
	people.push_back(new Person(999, "validation", "Faces\\validation\\gray", "Faces\\validation\\depth"));
	*/
	//Each person fills its image vectors with grays and depths and negatives and eventually validation

	//Cross positive training
	for (Person* person : people) 
	{
		int currentPerson = person->personID;
		if (currentPerson == 999) continue;
		
		//2 samples from each as negativo
		for (Person* other : people) 
		{
			if (currentPerson == other->personID) continue; //skip himself
			int getTwo = 0;
			for (Mat sample : other -> grayFaces) 
			{
				person->negativos.push_back(sample.clone());
				if (getTwo++ == 1) break;
			}
			getTwo = 0;
			for (Mat sample : other->depthFaces)
			{
				person->dnegativos.push_back(sample.clone());
				if (getTwo++ == 1) break;
			}
		}
	}

	//Test
	//for (int neu = 2; neu < 10; neu++) {
		for (Person* person : people) {
			if (person->personName == "validation") continue;
			person->trainGrayNN(6);
			//person->trainDepthNN(6);
		}
		int depthIndex = 0;
		//Depth on hold
		for (Mat validation: people[5] -> grayFaces) 
		{
			//Rect roi(20, 20, 60, 60);
			//Mat dvalidation = people[7] -> depthFaces[depthIndex++];
			//validation = validation(roi);
			//dvalidation = dvalidation(roi);
			Mat dvalidation;
			imshow("validation", validation);
			identificate(validation, dvalidation);
			waitKey(0);
		}
		cout << "para";
	//}


}

string MSN::identificate(cv::Mat input, cv::Mat dinput) {
	map<double, string> results;
	string result;

	for (Person* person : people) 
	{	
		if (person->trained) 
		{
			double result = person->predict(input, dinput);
			string name = person->personName;
			//cout << person->personName + " saida da NN: " + to_string(result);
			results.insert(pair<double, string >(result, name));
		}
		else cout << person -> personName + " não treinado.";

	}

	int count = 0;
	for (auto r : results) {
		cout << r.first << r.second << endl << endl;
		if (++count == results.size()) 
		{
			result = r.second;
		}
	}
	
	return result;
}