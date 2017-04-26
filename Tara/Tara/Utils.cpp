#include "stdafx.h"
#include "Utils.hpp"

using namespace std;

//Utility to check if an string ends with other string
bool ends_with(const std::string &suffix, const std::string &str)
{
	return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}