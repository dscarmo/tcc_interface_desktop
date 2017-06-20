#pragma once
#include <string>
#include <vector>
bool ends_with(const std::string &suffix, const std::string &str);

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> splitString(const std::string &s, char delim);