#include "stdafx.h"
#include "Utils.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
using namespace std;

//Utility to check if an string ends with other string
bool ends_with(const std::string &suffix, const std::string &str)
{
	return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}


std::vector<std::string> splitString(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}