#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;

// �v���g�^�C�v�錾
void extract_gray_block(Mat image);

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
	
	extract_gray_block(image);

	clock_t end = clock();

	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;

	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	return 0;
}

void extract_gray_block(Mat image) {
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //�O���[�X�P�[���ɕϊ�
	imshow("�O���[�X�P�[���摜", gray);

	// gray block feature�𒊏o����
	int n = 100; // �摜��n * n�̃u���b�N�ɕ�������
	Mat gray_block = Mat::zeros(n, n, CV_8U);
	Mat gray_block_sum = Mat::zeros(n, n, CV_64FC1);

	int gray_block_rows_length = gray.rows / n;
	int gray_block_cols_length = gray.cols / n;

	for (int y = 0; y < gray.rows; y++) {
		for (int x = 0; x < gray.cols; x++) {
			if (y < gray_block_rows_length * n && x < gray_block_cols_length * n) {
				gray_block_sum.at<double>(y / gray_block_rows_length, x / gray_block_cols_length) += +gray.at<unsigned char>(y, x);
			}
		}

	}

	for (int y = 0; y < n; y++) {
		for (int x = 0; x < n; x++) {
			gray_block.at<unsigned char>(y, x) = gray_block_sum.at<double>(y, x) / (gray_block_rows_length * gray_block_cols_length);
		}
	}

	imshow("�O���[�u���b�N�摜", gray_block);
}