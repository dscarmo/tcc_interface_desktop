#include "stdafx.h"
#include <dirent.h>
#include "multipleSmallNetworks.hpp"
#include "Utils.hpp"
#include "TDLBP.h"

using namespace std;
using namespace cv;

Person::Person(int id, string name, string grayPath){
	//Put arguments and pushback faces
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
	graynn->load<ml::ANN_MLP>(gray_xml_path);
}

void Person::loadSVM() {
	svm->load<ml::SVM>(svm_xml_path);
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
	
	points2d = (double)PESO*output.at<float>(0, 0)/3.4 + 0.5; // -1.7 to 1.7 needs normalize
	pointss = (double)PESOSVM*output.at<float>(0, 0)/2 + 0.5; //-1 to 1
	cout << "MLP de " + personName + ": " + to_string(points2d/PESO) << "SVM: " << to_string(pointss) << endl;
	
	

	return (points2d + pointss) / 0.02;
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
	for (int i = 1; i < 40; i++) 
	{
		//Excluded faces, too similar
		if (i != 2 &&
			i != 4 &&
			i != 5 &&
			i != 6 &&
			i != 7 &&
			i != 8 &&
			i != 13 &&
			i != 16 &&
			i != 17 &&
			i != 23 &&
			i != 26 &&
			i != 27 &&
			i != 31 &&
			i != 32 &&
			i != 33 &&
			i != 36  ) continue;//doesnt exist, dont know why
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
	
	//Fill validation
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
				}
			}
		}
	}

	//Yale Small
	/*people.push_back(new Person(1, "asian_woman", "Faces\\asian_woman\\gray"));
	people.push_back(new Person(2, "big_eye", "Faces\\big_eye\\gray"));
	people.push_back(new Person(3, "europeu", "Faces\\bigode_limpo\\gray"));
	people.push_back(new Person(5, "sobrancelha_asia", "Faces\\sobrancelha_asia\\gray"));
	people.push_back(new Person(999, "validation", "Faces\\validation\\gray"));

	people.push_back(new Person(6, "diedre", "Faces\\diedre\\gray"));

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
	}

	//Debug display filled images
	if (debugshow)
	for (Person* person : people) {
		person->displayPerson(100);
	}

	//Test
	//for (int neu = 2; neu < 10; neu++) {
	int validationIndex = 0;
	int i = 0;
	if (trainAgain)
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
	else
	{
		for (Person* person : people)
		{
			person->loadGrayNN();
			person->loadSVM();

		}
	}

	int depthIndex = 0;
	//Depth on hold
	int z = 0;
	for (Mat validation: people[validationIndex] -> grayFaces) 
	{
		//Rect roi(20, 20, 60, 60);
		//Mat dvalidation = people[7] -> depthFaces[depthIndex++];
		//validation = validation(roi);
		//dvalidation = dvalidation(roi);
		Mat dvalidation;

		identificate(validation);
		cout << people[z]->personName << endl;
		imshow("validation", validation);
		waitKey(0);

		z++;
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