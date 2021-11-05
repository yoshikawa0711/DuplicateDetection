#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;


int main()
{
	clock_t start = clock();

	Mat image =imread("img.jpg"); //�t�@�C���ǂݍ���
	if (image.empty()) {  //Mat�I�u�W�F�N�g����̂Ƃ�
		cout << "�t�@�C�����ǂݍ��߂܂���";
		cin.get();
		return -1;
	}
	imshow("�J���[�摜", image);
	
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //�O���[�X�P�[���ɕϊ�
	imshow("�O���[�X�P�[���摜", gray);

	// gray�ɂ��Ē��ׂ邽�߂̏o��
	// cout << "dims: " << gray.dims << endl;
	// cout << "depth: " << gray.elemSize1() << endl;
	// cout << "channel: " << gray.channels() << endl;
	// cout << "rows: " << gray.rows << endl;
	// cout << "cols: " << gray.cols << endl;
	// cout << "width: " << gray.size().width << ", height: " << gray.size().height << endl;
	// cout << "I(x, y): " << +gray.at<unsigned char>(gray.rows-1, gray.cols-1) << endl;


	// gray block feature�𒊏o����
	int n = 100; // �摜��n*n�̃u���b�N�ɕ�������
	Mat gray_block = Mat::zeros(n, n, CV_8U);
	Mat gray_block_sum = Mat::zeros(n, n, CV_64FC1);

	// gray_block�ɂ��Ē��ׂ邽�߂̏o��
	// cout << "dims: " << gray_block.dims << endl;
	// cout << "depth: " << gray_block.elemSize1() << endl;
	// cout << "channel: " << gray_block.channels() << endl;
	// cout << "rows: " << gray_block.rows << endl;
	// cout << "cols: " << gray_block.cols << endl;
	// cout << "(x, y): " << +gray_block.at<unsigned char>(0, 0) << endl;

	int gray_block_rows_length = gray.rows / n;
	int gray_block_cols_length = gray.cols / n;

	// cout << "rows length: " << gray_block_rows_length << ", cols length: " << gray_block_cols_length << endl;

	for (int y = 0; y < gray.rows; y++) {
		for (int x = 0; x < gray.cols; x++ ) {
			if (y < gray_block_rows_length * n && x < gray_block_cols_length * n) {
				gray_block_sum.at<double>(y / gray_block_rows_length, x / gray_block_cols_length) += +gray.at<unsigned char>(y, x);
				// gray_block.at<unsigned char>(y / gray_block_rows_length, x / gray_block_cols_length) += gray.at<unsigned char>(y, x) / (gray_block_rows_length * gray_block_cols_length);
			}
		}

	}

	for (int y = 0; y < n; y++) {
		for (int x = 0; x < n; x++) {
			gray_block.at<unsigned char>(y, x) = gray_block_sum.at<double>(y, x) / (gray_block_rows_length * gray_block_cols_length);
		}
	}


	imshow("�O���[�u���b�N�摜", gray_block);

	clock_t end = clock();

	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;

	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	return 0;
}
