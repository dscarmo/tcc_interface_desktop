#include "stdafx.h"
#include <dirent.h>
#include "NeuralNetwork.hpp"
#include "LocalBinaryPattern.hpp"
#include "iostream"

using namespace std;
using namespace cv;
LocalBinaryPattern global_lbp;




//Utility to check if an string ends with other string
bool ends_with(const std::string &suffix, const std::string &str)
{
	return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


int runFromDirectory() {
	//New classe incendio interno

	//Generic Dataset run
	cout << "Rodando apartir de arquivos de imagens..." << endl;
	//int contador = 0;

	Mat image;
	//int resultado;
	vector<String> all_files;
	

	//Onde ta as imagem
	
	//const char* input_dir = "Faces";
	//const char* input_dir = "Faces\\diedre_night";
	const char* input_dir = "Faces\\diedre\\gray";

	//Pointer to input directory
	DIR *dir_ptr = opendir(input_dir);
	if (dir_ptr == NULL) {
		cout << "DIR NULL POINTER!!! Insert correct hardcoded dataset directory" << endl;
		system("PAUSE");
		return 1;
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
			image_files.push_back(file);
		}
	}
	// ----------------------------------------------- 

	//Main loop
	cout << input_dir << "diretorio sendo processado" << endl;

	//Primeira imagem vira background
	for (String file : image_files) {
		cout << input_dir + file << endl;
		ostringstream oss;
		oss << input_dir << "\\" << file;
		image = imread(oss.str(), 0);

		if (!image.empty())
			cout << "Imagem lida com sucesso." << endl;
		else {
			cout << "Erro na leitura de imagem, continuando." << endl;
			continue;
		}

		imshow("hd image", image);

		cout << "processando: " << file << endl;
		global_lbp.grayLBPpipeline(image);
		global_lbp.depthPipeline(image);
		waitKey(0);
	}

	return 0;
}
