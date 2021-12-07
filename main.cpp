#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;

// �v���g�^�C�v�錾
Mat extract_gray_block(Mat image);
void count_diff(Mat image01_gray, Mat image02_gray);

int main()
{
	clock_t start = clock();

	Mat image01 =imread("img-01.jpg"); //�t�@�C���ǂݍ���
	if (image01.empty()) {  //Mat�I�u�W�F�N�g����̂Ƃ�
		cout << "�t�@�C�����ǂݍ��߂܂���";
		cin.get();
		return -1;
	}
	imshow("�J���[�摜1", image01);
	
	Mat image02 = imread("img-08.jpg"); //�t�@�C���ǂݍ���
	if (image02.empty()) {  //Mat�I�u�W�F�N�g����̂Ƃ�
		cout << "�t�@�C�����ǂݍ��߂܂���";
		cin.get();
		return -1;
	}
	imshow("�J���[�摜2", image02);
	
	// �O���[�u���b�N�����̒��o
	Mat image01_gray = extract_gray_block(image01);
	Mat image02_gray = extract_gray_block(image02);

	imshow("�O���[�u���b�N�摜1", image01_gray);
	imshow("�O���[�u���b�N�摜2", image02_gray);

	// �O���[�u���b�N�̍����J�E���g
	count_diff(image01_gray, image02_gray);

	clock_t end = clock();

	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;

	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	return 0;
}

Mat extract_gray_block(Mat image) {
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //�O���[�X�P�[���ɕϊ�
	
	// TODO: �t�@�C�������󂯎���ĕ����̃O���[�X�P�[���摜��\���ł���悤�ɂ���
	// imshow("�O���[�X�P�[���摜", gray);

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

	return gray_block;
}

void count_diff(Mat image01_gray, Mat image02_gray) {
	// �O���[�u���b�N�����̒l�̈Ⴄ�s�N�Z�����J�E���g
	int color_threshold = 2;
	int count_diff = 0;
	int count_threshold = 5;

	for (int y = 0; y < image01_gray.rows; y++) {
		for (int x = 0; x < image01_gray.cols; x++) {
			if ((image01_gray.at<unsigned char>(y, x) < image02_gray.at<unsigned char>(y, x) - color_threshold) || (image02_gray.at<unsigned char>(y, x) + color_threshold < image01_gray.at<unsigned char>(y, x))) {
				cout << "image01: " << +image01_gray.at<unsigned char>(y, x);
				cout << ", image02: " << +image02_gray.at<unsigned char>(y, x) << endl;
				count_diff++;
			}
		}

	}

	if (count_diff >= count_threshold) {
		cout << count_diff << "�s�N�Z�����Ⴄ���߁C�d���摜�ł͂���܂���D" << endl;
	}
	else {
		cout << count_diff << "�s�N�Z�����Ⴄ���߁C�d���摜�ł��D" << endl;
	}
}