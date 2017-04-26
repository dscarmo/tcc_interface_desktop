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
	fillVector(grayFaces, grayPath);
	fillVector(depthFaces, depthPath);

	for (int i = 0; i < grayFaces.size(); i++) {
		imshow("gray faces", grayFaces[i]);
	}

	for (int j = 0; j < depthFaces.size(); j++) {
		imshow("depthFaces", depthFaces[j]);
	}

}


void Person::fillVector(vector<Mat> faces, const char* input_dir) {
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

	vector<String> image_files;
	for (String file : all_files) {
		if (ends_with(".png", file) || ends_with(".jpg", file) || ends_with(".jpeg", file)) {
			ostringstream oss;
			oss << input_dir << "\\" << file;
			faces.push_back(imread(oss.str()));
		}
	}
	// ----------------------------------------------- 

	//Main loop
	cout << input_dir << "diretorio sendo processado" << endl;

	for (Mat face : faces) {
		

		if (!face.empty())
			cout << "Imagem lida com sucesso." << endl;
		else {
			cout << "Erro na leitura de imagem, continuando." << endl;
			continue;
		}

		waitKey(1);
	}

}

MSN::MSN() {
	people.push_back(Person(1, "diedre", "Faces\\diedre\\gray", "Faces\\diedre\\depth"));
}