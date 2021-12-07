#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;

// プロトタイプ宣言
Mat extract_gray_block(Mat image);

int main()
{
	clock_t start = clock();

	Mat image01 =imread("img-01.jpg"); //ファイル読み込み
	if (image01.empty()) {  //Matオブジェクトが空のとき
		cout << "ファイルが読み込めません";
		cin.get();
		return -1;
	}
	imshow("カラー画像1", image01);
	
	Mat image02 = imread("img-08.jpg"); //ファイル読み込み
	if (image02.empty()) {  //Matオブジェクトが空のとき
		cout << "ファイルが読み込めません";
		cin.get();
		return -1;
	}
	imshow("カラー画像2", image02);
	
	// グレーブロック特徴の抽出
	Mat image01_gray = extract_gray_block(image01);
	Mat image02_gray = extract_gray_block(image02);

	imshow("グレーブロック画像1", image01_gray);
	imshow("グレーブロック画像2", image02_gray);

	// グレーブロック特徴の値の違うピクセルをカウント
	int color_threshold = 2;
	int count_diff = 0; 
	int count_threshold = 5;

	for (int y = 0; y < image01_gray.rows; y++) {
		for (int x = 0; x < image01_gray.cols; x++) {
			if ((image01_gray.at<unsigned char>(y, x) < image02_gray.at<unsigned char>(y, x) - color_threshold ) || (image02_gray.at<unsigned char>(y, x) + color_threshold < image01_gray.at<unsigned char>(y, x))) {
				cout << "image01: " << +image01_gray.at<unsigned char>(y, x);
				cout << ", image02: " << +image02_gray.at<unsigned char>(y, x) << endl;
				count_diff++;
			}
		}

	}

	if (count_diff >= count_threshold) {
		cout << count_diff << "個ピクセルが違うため，重複画像ではありません．" << endl;
	}
	else {
		cout << count_diff << "個ピクセルが違うため，重複画像です．" << endl;
	}

	clock_t end = clock();

	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;

	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	return 0;
}

Mat extract_gray_block(Mat image) {
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //グレースケールに変換
	
	// TODO: ファイル名を受け取って複数のグレースケール画像を表示できるようにする
	// imshow("グレースケール画像", gray);

	// gray block featureを抽出する
	int n = 100; // 画像をn * nのブロックに分割する
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