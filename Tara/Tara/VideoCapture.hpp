#pragma once

int capture();
int record_frame(cv::Mat frame, string name, int width, int height);
void getSize(int* width, int* height);