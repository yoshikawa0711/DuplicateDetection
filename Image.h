#pragma once

#include <opencv2/opencv.hpp>
#include <string>
using namespace cv;
using namespace std;

#ifndef IMAGE_H
#define IMAGE_H

class Image {
	private:
		string file_name;
		Mat image;
		Mat gray_image;
		Mat gray_block;

	public:
		Image();
		Image(string file_name, Mat image);
		void generateGrayBlock(int size);
		bool isEmpty();
		string getFileName();
		Mat getImage();
		Mat getGrayBlock();

};

#endif
