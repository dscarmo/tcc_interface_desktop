#pragma once
#include <string>
#include <vector>
#include <chrono>
bool ends_with(const std::string &suffix, const std::string &str);

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> splitString(const std::string &s, char delim);


class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<second_>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};