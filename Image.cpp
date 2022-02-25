#include "Image.h"

Image::Image() {

}

Image::Image(string file_name, Mat image) {
	this->file_name = file_name;
	this->image = image;
	cvtColor(image, this->gray_image, COLOR_BGR2GRAY);
}

void Image::generateGrayBlock(int size) {

	this->gray_block = Mat::zeros(size, size, CV_8U);
	Mat gray_block_sum = Mat::zeros(size, size, CV_32S);
	Mat gray_block_count = Mat::zeros(size, size, CV_32S);

	double gray_block_rows_length = 1.0 * this->gray_image.rows / size;
	double gray_block_cols_length = 1.0 * this->gray_image.cols / size;

	// 該当範囲にあるピクセルの値を足し合わせる
	for (int y = 0; y < this->gray_image.rows; y++) {
		for (int x = 0; x < this->gray_image.cols; x++) {
			if (y < gray_block_rows_length * size && x < gray_block_cols_length * size) {
				if ((int)round(y / gray_block_rows_length) < size && (int)round(x / gray_block_cols_length) < size) {
					gray_block_sum.at<int>((int)round(y / gray_block_rows_length), (int)round(x / gray_block_cols_length)) += +this->gray_image.at<unsigned char>(y, x);
					gray_block_count.at<int>((int)round(y / gray_block_rows_length), (int)round(x / gray_block_cols_length)) += 1;
				}
				else if ((int)round(y / gray_block_rows_length) < size) {
					gray_block_sum.at<int>((int)round(y / gray_block_rows_length), size - 1) += +this->gray_image.at<unsigned char>(y, x);
					gray_block_count.at<int>((int)round(y / gray_block_rows_length), size - 1) += 1;
				}
				else if ((int)round(x / gray_block_cols_length) < size) {
					gray_block_sum.at<int>(size - 1, (int)round(x / gray_block_cols_length)) += +this->gray_image.at<unsigned char>(y, x);
					gray_block_count.at<int>(size - 1, (int)round(x / gray_block_cols_length)) += 1;
				}

			}
		}

	}

	// ピクセルの値の平均を求める
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (gray_block_count.at<int>(y, x) != 0) {
				gray_block.at<unsigned char>(y, x) = gray_block_sum.at<int>(y, x) / gray_block_count.at<int>(y, x);
			}
		}
	}
}

bool Image::isEmpty() {
	return this->image.empty();
}

string Image::getFileName() {
	return this->file_name;
}

Mat Image::getImage() {
	return this->image;
}

Mat Image::getGrayBlock() {
	return this->gray_block;
}