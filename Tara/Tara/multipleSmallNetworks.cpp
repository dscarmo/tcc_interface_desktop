#include "stdafx.h"
#include <dirent.h>
#include <thread>
#include <vector>
#include <fstream>
#include "multipleSmallNetworks.hpp"
#include "Utils.hpp"
#include "TDLBP.h"

using namespace std;
using namespace cv;

Person::Person(int id, string name, string grayPath){
	//Put arguments and pushback faces
	wrote = false;
	trained = false;
	personID = id;
	personName = name;
	Mat readGray;
	
	//Fill vector gray
	fillVector(grayPath);


	gray_xml_path = "xml\\gray_neural_" + personName + ".xml";
	svm_xml_path = "xml\\svm_neural_" + personName + ".xml";

}

void Person::loadGrayNN() {
	graynn = Algorithm::load<ml::ANN_MLP>(gray_xml_path);
}

void Person::loadSVM() {
	svm = Algorithm::load<ml::SVM>(svm_xml_path);
	setStatus(TRAINED);
}

void Person::displayPerson(int waittime) {
	for (Mat face : grayFaces) 
	{
		imshow("gray faces", face);
		waitKey(waittime);
	}

	for (Mat face : negativos) {
		imshow("negativos", face);
		waitKey(waittime);
	}
}

//Saving to file for asian
void Person::saveFeatures() {
	ofstream file;

	file.open("text\\p" + personName + ".txt");
	for (auto positivo : grayFaces) {
		Mat feature = test_lbp.betterLBPpipeline(positivo);
		for (int i = 0; i < feature.rows; i++) {
			for (int j = 0; j < feature.cols; j++) {
				file << feature.at<float>(i, j) << " ";
			}
		}
		file << endl;
	}
	file.close();

	file.open("text\\n" + personName + ".txt");
	for (auto positivo : grayFaces) {
		Mat feature = test_lbp.betterLBPpipeline(positivo);
		for (int i = 0; i < feature.rows; i++) {
			for (int j = 0; j < feature.cols; j++) {
				file << feature.at<float>(i, j) << " ";
			}
		}
		file << endl;
	}

	file.close();
	setStatus(WROTE);
}

int Person::trainGrayNN() {
	Mat layers = Mat(3, 1, CV_32S);
	layers.row(0) = Scalar(FEATURE_SIZE);
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

	Mat samples(Size(FEATURE_SIZE, grayFaces.size() + negativos.size()), CV_32F); //Positivos mais negativos
	Mat responsesvm(Size(1, grayFaces.size() + negativos.size()), CV_32S);
	Mat responses(Size(1, grayFaces.size() + negativos.size()), CV_32F);
	int contador = 0;

	//Positivos
	for (Mat positivo : grayFaces) {
		//Mat trainingHist = test_lbp.grayLBPpipeline(positivo);
		Mat trainingHist = test_lbp.betterLBPpipeline(positivo);
		trainingHist.row(0).copyTo(samples.row(contador));
		responses.at<float>(Point(0, contador)) = 1;
		responsesvm.at<float>(Point(0, contador)) = 1;
		contador++;
	}

	//Negativos
	//take one face from each other
	for (Mat negativo : negativos) { //Must get a negative dataset for faces
		//Mat trainingHist = test_lbp.grayLBPpipeline(negativo);
		Mat trainingHist = test_lbp.betterLBPpipeline(negativo);
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
	svm->train(t);
	destroyAllWindows();*/
	//cout << personName <<" - Training SVM..." << endl;
	svm->setKernel(ml::SVM::LINEAR);
	svm-> trainAuto(t);

	//Train mlp
	//cout << personName << " - Training MLP..." << endl;
	if (!graynn->train(samples, ml::ROW_SAMPLE, responses))
		return CREATION_ERROR;

	setStatus(TRAINED);
	//cout << "Treino concluído" << endl;
	//	svm -> save(svm_xml_path);
	try
	{
		//cout << "Saving NN train...";
		graynn->save(gray_xml_path);
		//cout << "Saving SVM train...";
		svm->save(svm_xml_path);
	}
	catch (const std::exception err)
	{
		cout << "Saving error: " << err.what() << endl;
	}
	//
	
	//cout << personName + " treinada." << endl;
	return TREINO_COMPLETO;
}

double Person::predict(Mat input) {
	Mat feature(Size(FEATURE_SIZE, 1), CV_32F), output, soutput;
	//Mat input_lbp = test_lbp.grayLBPpipeline(input);
	Mat input_lbp = test_lbp.betterLBPpipeline(input);
	input_lbp.row(0).copyTo(feature.row(0));
	double points2d, pointss;
	points2d = 0;
	pointss = 0;
	double PESO = 0.9;
	double PESOSVM = 0.1;

	//Gray
	graynn->predict(feature, output);
	svm->predict(feature, soutput);
	
	// Parameters
	//cout << "c: " << svm->getC() << " Epsilon: " << svm->getP() << " Kernel: " << svm->getKernelType() << " Type " << svm->getType();
	


	points2d = (double)output.at<float>(0, 0)/3.4 + 0.5; // -1.7 to 1.7 needs normalize to 0  - 1
	pointss = (double)soutput.at<int>(0, 0); //numero doido
	cout << "Número doido: " << pointss << endl;
	pointss > 0 ? pointss = 1 : pointss = 0; //says if svms agrees or not
	cout << "MLP de " + personName + ": " + to_string(points2d/PESO) << "SVM: " << to_string(pointss) << endl;
	
	
	//SVM adds 25% 
	return (PESO*points2d + PESOSVM*pointss)/0.01;
	//return (points2d) / 0.01;
	//return (pointss)/0.01;
}

//Fills with pre process
void Person::fillVector(string input_dir) {
	vector<String> all_files;

	//Pointer to input directory
	DIR *dir_ptr = opendir(input_dir.c_str());
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
		if (ends_with(".png", file) || ends_with(".jpg", file) || ends_with(".jpeg", file) || ends_with(".pgm",file) ) {
			
			//Check if yale dataset
			try
			{
				vector<string> splited = splitString(file, 'B');
				if (splited[0].compare("yale") == 0)
				{
					//Exclude high E images
					/*splited = splitString(file, '+');
					splited = splitString(splited[1], 'E');*/
					string substring = file.substr(13, 3);
					int exposure = stoi(substring);
					if (exposure > 20)
						continue;
				}
			}
			catch (const std::exception& e)
			{
				cout << e.what() << endl;
				continue;
			}
			
			


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
	//Yale full
	for (int i = 1; i < 5; i++) 
	{
		
		if (i == 14) continue;
		string index = "";
		string path;
		ostringstream ossindex, osspath;

		if (i < 10) 
		{
			ossindex << "0" << i;
		}
		else
		{	
			ossindex << i;
		}
		index = ossindex.str();
		osspath << "CroppedYale\\yaleB" << index;
		path = osspath.str();
		cout << path<< endl;
		people.push_back(new Person(i, "yaleB" + index, path));
	}

	people.push_back(new Person(999, "validation", "Faces\\validation"));
	
	//Put some faces on the validation (they are removed from positives for real)
	for (Person* validation : people)
	{
		if (validation->personID == 999) 
		{
			for (Person* person : people)
			{
				if (person->personID != 999)
				{
					validation->grayFaces.push_back(person->grayFaces.back());
					person->grayFaces.pop_back();
					validation->grayFaces.push_back(person->grayFaces.back());
					person->grayFaces.pop_back();
				}
			}
		}
	}

	//Cross positive training
	for (Person* person : people) 
	{
		int currentPerson = person->personID;
		if (currentPerson == 999) continue;
		
		//2 samples from each as negativo
		for (Person* other : people) 
		{
			//Skip odd
			if (currentPerson == other->personID) continue; //skip himself
			int getTwo = 0;
			for (Mat sample : other -> grayFaces) 
			{
				person->negativos.push_back(other->grayFaces[0].clone()); //only one negativo from each
				if (getTwo++ == 1) break;
			}
			getTwo = 0;
		}
		cout << "Person name: " << person->personName << endl;
		cout << "Number of Positives: " << person->grayFaces.size() << endl;
		cout << "Number of Negatives: " << person->negativos.size() << endl;

	}
	Sleep(1000);
	
	//Fill text files features
	cout << "Writing features to files with parallel pool of threads... This will take a while..." << endl;
	vector<thread *> writers;
	completed = 0;
	for (auto person : people) {
		if (person->personName == "validation") continue;
		completed = 0;
		for (auto person : people) if (person->getStatus(WROTE)) completed++;
		loadingBar("Starting write for..." + person->personName, (completed * 100) / people.size());
		writers.push_back(new thread(&Person::saveFeatures, person));
		
	}
	while (completed < people.size() - 1) {
		completed = 0;
		for (auto person : people) if (person->getStatus(WROTE)) completed++;
		loadingBar("Writing... ", (completed * 100) / people.size());
		Sleep(1000);
	}
	for (auto w : writers) {
		w->join();
		
	}

	//Debug display filled images
	if (debugshow)
	for (Person* person : people) {
		person->displayPerson(100);
	}

	//Threaded classifier generation
	int validationIndex = 0;
	int i = 0;
	vector<thread *> threads;
	if (trainAgain) {
		cout << "Training classifiers with parallel pool of threads... This will take a while." << endl;
		completed = 0;
		for (Person* person : people)
		{
			if (person->personName == "validation")
			{
				validationIndex = i;
				continue;
			}
			i++;
			completed = 0;
			for (auto person : people) if (person->getStatus(TRAINED)) completed++;
			loadingBar("Starting training for..." + person->personName, (completed * 100) / people.size());
			threads.push_back(new thread(&Person::trainGrayNN, person));
		}
		while (completed < people.size() - 1) {
			completed = 0;
			for (auto person : people) if (person->getStatus(TRAINED)) completed++;
			loadingBar("Training... ", (completed * 100) / people.size());
			Sleep(2000);
		}
		for (auto t : threads) {
			t->join();
		}
	}
	else
	{
		cout << "Loading Classifiers with parallel pool of threads..." << endl;
		for (Person* person : people)
		{
			if (person->personName == "validation") 
			{ 
				validationIndex = i;
				continue; 
			}
			completed = 0;
			for (auto person : people) if (person->getStatus(TRAINED)) completed++;
			loadingBar("Starting loading for..." + person->personName, (completed * 100) / people.size());
			threads.push_back(new thread(&Person::loadGrayNN, person));
			threads.push_back(new thread(&Person::loadSVM, person));
			i++;
			
		}
		completed = 0;
		while (completed < people.size() - 1) {
			completed = 0;
			for (auto person : people) if (person->getStatus(TRAINED)) completed++;
			loadingBar("Loading... ", (completed * 100) / people.size());
			Sleep(100);
		}
		for (auto t : threads) { 
			t->join();
		}
	}

	//Validation test
	int z = 0;
	int x = 0;
	for (Mat validation: people[validationIndex] -> grayFaces) 
	{
		Mat dvalidation;

		identificate(validation);
		cout << people[x]->personName << endl;
		imshow("validation", validation);
		waitKey(0);

		z++;
		if (z % 2 == 0) x++;
	}

	destroyAllWindows();
	Sleep(1000);


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
	cout << "Degrees of similarity: " << endl;
	for (auto r : results) {
		cout << r.first << " %" << "Person: " << r.second << endl << endl;
		if (++count == results.size()) 
		{
			result = r.second;
		}
	}
	
	return result;
}