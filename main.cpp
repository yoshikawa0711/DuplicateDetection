#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;

// �v���g�^�C�v�錾
Mat extract_gray_block(Mat image, int size);
void count_diff(Mat image01_gray, Mat image02_gray);
void change_n_value(int size, void* userdata);

// �O���[�o���ϐ�
Mat image01 = imread("img-01.jpg"); //�t�@�C���ǂݍ���

int main()
{
	clock_t start = clock();
	int n = 100;

	if (image01.empty()) {  //Mat�I�u�W�F�N�g����̂Ƃ�
		cout << "�t�@�C�����ǂݍ��߂܂���";
		cin.get();
		return -1;
	}
	imshow("�J���[�摜1", image01);
	
	Mat image02 = imread("img-05.jpg"); //�t�@�C���ǂݍ���
	if (image02.empty()) {  //Mat�I�u�W�F�N�g����̂Ƃ�
		cout << "�t�@�C�����ǂݍ��߂܂���";
		cin.get();
		return -1;
	}
	imshow("�J���[�摜2", image02);
	
	// �O���[�u���b�N�����̒��o
	Mat image01_gray = extract_gray_block(image01, 100);
	Mat image02_gray = extract_gray_block(image02, 100);

	imshow("�O���[�u���b�N�摜1", image01_gray);
	imshow("�O���[�u���b�N�摜2", image02_gray);

	int max_n = 0;
	// �摜�̏c���̒����̂����A�Z���ق��� n �̍ő�l�Ƃ���
	if (image01.rows < image01.cols) {
		max_n = image01.rows;
	}
	else {
		max_n = image01.cols;
	}

	namedWindow("gray block image 1", WINDOW_NORMAL);
	createTrackbar("Divison n", "gray block image 1", NULL, max_n, change_n_value);
	setTrackbarPos("Divison n", "gray block image 1", n);


	// �O���[�u���b�N�̍����J�E���g
	count_diff(image01_gray, image02_gray);

	// ���s�ɂ����������Ԃ��Z�o
	clock_t end = clock();
	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;
	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	destroyAllWindows();
	return 0;
}

Mat extract_gray_block(Mat image, int size) {
	// TODO: �Ȃ񂩏c�Ɖ��̊֌W�����܂������ĂȂ��݂����Ȃ̂ŏC������

	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //�O���[�X�P�[���ɕϊ�
	
	// TODO: �t�@�C�������󂯎���ĕ����̃O���[�X�P�[���摜��\���ł���悤�ɂ���
	// imshow("�O���[�X�P�[���摜", gray);

	// gray block feature�𒊏o����
	Mat gray_block = Mat::zeros(size, size, CV_8U);
	Mat gray_block_sum = Mat::zeros(size, size, CV_32S);
	Mat gray_block_count = Mat::zeros(size, size, CV_32S);

	double gray_block_rows_length = 1.0 * gray.rows / size;
	double gray_block_cols_length = 1.0 * gray.cols / size;

	for (int y = 0; y < gray.rows; y++) {
		for (int x = 0; x < gray.cols; x++) {
			if (y < gray_block_rows_length * size && x < gray_block_cols_length * size) {
				if ((int)round(y / gray_block_rows_length) < size && (int)round(x / gray_block_cols_length) < size) {
					gray_block_sum.at<int>((int)round(y / gray_block_rows_length), (int)round(x / gray_block_cols_length)) += +gray.at<unsigned char>(y, x);
					gray_block_count.at<int>((int)round(y / gray_block_rows_length), (int)round(x / gray_block_cols_length)) += 1;
				} 
				else if ((int)round(y / gray_block_rows_length) < size) {
					gray_block_sum.at<int>((int)round(y / gray_block_rows_length), size - 1) += +gray.at<unsigned char>(y, x);
					gray_block_count.at<int>((int)round(y / gray_block_rows_length), size - 1) += 1;
				}
				else if ((int)round(x / gray_block_cols_length) < size) {
					gray_block_sum.at<int>(size - 1, (int)round(x / gray_block_cols_length)) += +gray.at<unsigned char>(y, x);
					gray_block_count.at<int>(size - 1, (int)round(x / gray_block_cols_length)) += 1;
				}

			}
		}

	}

	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (gray_block_count.at<int>(y, x) == 0) {
				cout << "x: " << x << ", y: " << y << endl;
			}
			else {
				gray_block.at<unsigned char>(y, x) = gray_block_sum.at<int>(y, x) / gray_block_count.at<int>(y, x);
			}
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

void change_n_value(int size, void* userdata) {
	cout << "���݂�n: " << size << endl;

	Mat image01_gray;

	if (size > 0) {
		image01_gray = extract_gray_block(image01, size);
	}
	else {
		image01_gray = extract_gray_block(image01, 1);
	}

	imshow("gray block image 1", image01_gray);
}