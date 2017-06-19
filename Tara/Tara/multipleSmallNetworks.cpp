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
	svm_xml_path = "xml\\svm_neural_" + personName + ".xml";
	//Fill vector gray
	//Fill vector depth
	fillVector(grayPath);

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

int Person::trainGrayNN(int neuronios) {
	Mat layers = Mat(3, 1, CV_32S);
	layers.row(0) = Scalar(256);
	layers.row(1) = Scalar(neuronios);
	layers.row(2) = Scalar(1);
	
	//svm
	svm = ml::SVM::create();


	//neural
	graynn = ml::ANN_MLP::create();
	graynn -> setLayerSizes(layers);
	graynn -> setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 0, 0);
	graynn -> setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	graynn -> setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);

	Mat samples(Size(256, grayFaces.size() + negativos.size()), CV_32F); //Positivos mais negativos
	Mat responsesvm(Size(1, grayFaces.size() + negativos.size()), CV_32S);
	Mat responses(Size(1, grayFaces.size() + negativos.size()), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : grayFaces) {
		Mat trainingHist = test_lbp.grayLBPpipeline(positivo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		responsesvm.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	//take one face from each other
	for (Mat negativo : negativos) { //Must get a negative dataset for faces
		Mat trainingHist = test_lbp.grayLBPpipeline(negativo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = -1;
		responsesvm.at<float>(Point(0, contador)) = -1;
		contador++;
	}

	//Train svm
	Ptr<ml::TrainData> t = cv::ml::TrainData::create(samples, ml::ROW_SAMPLE, responsesvm);
	/*svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	svm->train(t);*/
	svm-> trainAuto(t);

	//Train mlp
	if (!graynn->train(samples, ml::ROW_SAMPLE, responses))
		return CREATION_ERROR;

	trained = true;
	//cout << "Treino concluído" << endl;
	svm -> save(svm_xml_path);
	graynn -> save(gray_xml_path);
	cout << "rede neural de " + personName + " treinada..." << endl;
	return TREINO_COMPLETO;
}

double Person::predict(Mat input) {
	Mat feature(Size(256, 1), CV_32F), output, soutput;
	Mat input_lbp = test_lbp.grayLBPpipeline(input);
	input_lbp.row(0).copyTo(feature.row(0));
	double points2d, pointss;
	points2d = 0;
	int PESO = 1;
	int PESOSVM = 1;

	//Gray
	graynn->predict(feature, output);
	svm->predict(feature, soutput);
	
	points2d = (double)PESO*output.at<float>(0, 0);
	pointss = (double)PESOSVM*output.at<float>(0, 0);
	cout << "MLP de " + personName + ": " + to_string(points2d/PESO) << "SVM: " << to_string(pointss) << endl;
	
	

	return points2d + pointss;
}

//Fills with pre process
void Person::fillVector(const char* input_dir) {
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
			//Mat image;
			//resize(imread(oss.str(), 0),image, Size(100,100));
			Mat image = imread(oss.str(),0);
			equalizeHist(image, image);
			if (!image.empty())
				cout << "Imagem lida com sucesso." << endl;
			else 
			{
				cout << "Erro na leitura de imagem, continuando." << endl;
				continue;
			}
			grayFaces.push_back(image.clone());
		}
	}


}

MSN::MSN() {

	//Yale
	people.push_back(new Person(1, "asian_woman", "Faces\\asian_woman\\gray", "Faces\\asian_woman\\depth"));
	people.push_back(new Person(2, "big_eye", "Faces\\big_eye\\gray", "Faces\\big_eye\\depth"));
	people.push_back(new Person(3, "europeu", "Faces\\bigode_limpo\\gray", "Faces\\bigode_limpo\\depth"));
	//people.push_back(new Person(4, "bigode_marcado", "Faces\\bigode_marcado\\gray", "Faces\\bigode_marcado\\depth"));
	people.push_back(new Person(5, "sobrancelha_asia", "Faces\\sobrancelha_asia\\gray", "Faces\\sobrancelha_asia\\depth"));
	people.push_back(new Person(999, "validation", "Faces\\validation\\gray", "Faces\\validation\\depth"));

	people.push_back(new Person(6, "diedre", "Faces\\diedre\\gray", "Faces\\diedre\\depth"));

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
		}
	}

	//Test
	//for (int neu = 2; neu < 10; neu++) {
	int validationIndex = 0;
	int i = 0;
	for (Person* person : people) 
	{
		if (person->personName == "validation") 
		{
			validationIndex = i;
			continue;
		} 
		i++;
		person->trainGrayNN(6);
		//person->trainDepthNN(6);
	}

	int depthIndex = 0;
	//Depth on hold
		
	for (Mat validation: people[validationIndex] -> grayFaces) 
	{
		//Rect roi(20, 20, 60, 60);
		//Mat dvalidation = people[7] -> depthFaces[depthIndex++];
		//validation = validation(roi);
		//dvalidation = dvalidation(roi);
		Mat dvalidation;
		imshow("validation", validation);
		identificate(validation);
		waitKey(0);
	}
	cout << "para";


}

string MSN::identificate(cv::Mat input) {
	map<double, string> results;
	string result;

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