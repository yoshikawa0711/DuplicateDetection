#include <opencv2/opencv.hpp>
#include <time.h>
using namespace cv;
using namespace std;

// プロトタイプ宣言
// 重複除去にかかわる関数
Mat extractGrayBlock(Mat image, int size);
void showTwoImages(Mat image01, Mat image02);
int countDiffPixels(Mat image01_gray, Mat image02_gray, int new_color_threshold);
bool isDuplicateImage(int count_diff_pixels, int new_count_threshold);

// 編集履歴の分析に必要な関数
void analyseEditingHistory(Mat image01, Mat image02);

// トラックバーの値に応じて処理をし直すための関数
void changeNValue(int size, void* userdata);
void changeColorValue(int color, void* userdata);
void changeCountValue(int count, void* userdata);

// その他の計算に必要な関数
int minTwoImagesEdgs(Mat image01, Mat image02);

// グローバル変数
Mat image01 = imread("img/img-01.jpg"); //ファイル読み込み
Mat image01_gray;

Mat image02 = imread("img/img-10.jpg"); //ファイル読み込み
Mat image02_gray;

int main()
{
	clock_t start = clock();

	if (image01.empty()) {  //Matオブジェクトが空のとき
		cout << "ファイルが読み込めません";
		cin.get();
		return -1;
	}
	imshow("カラー画像1", image01);
	
	if (image02.empty()) {  //Matオブジェクトが空のとき
		cout << "ファイルが読み込めません";
		cin.get();
		return -1;
	}
	imshow("カラー画像2", image02);

	int division_n = 50;
	// 画像の縦横の長さのうち、短いほうを n の最大値とする
	int max_n = minTwoImagesEdgs(image01, image02);

	namedWindow("gray block images", WINDOW_NORMAL);
	createTrackbar("Divison n", "gray block images", NULL, max_n, changeNValue);
	setTrackbarPos("Divison n", "gray block images", division_n);
	
	int color = 16;
	createTrackbar("Allowable Error color", "gray block images", NULL, 255, changeColorValue);
	setTrackbarPos("Allowable Error color", "gray block images", color);
	
	int count = division_n * division_n * 0.01;
	createTrackbar("Allowable Error count", "gray block images", NULL, division_n * division_n, changeCountValue);
	setTrackbarPos("Allowable Error count", "gray block images", count);


	// 実行にかかった時間を算出
	clock_t end = clock();
	const double time = static_cast<double> (end - start) / CLOCKS_PER_SEC * 1.0;
	cout << "time: " << time << "[sec]" << endl;

	waitKey();
	destroyAllWindows();
	return 0;
}

/// <summary>
///	グレーブロック特徴を抽出するプログラム
/// </summary>
/// <param name="image">グレーブロック特徴を抽出したい画像</param>
/// <param name="size">抽出するグレーブロック特徴の縦・横のピクセル数</param>
/// <returns>抽出したグレーブロック特徴</returns>
Mat extractGrayBlock(Mat image, int size) {

	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);  //グレースケールに変換

	Mat gray_block = Mat::zeros(size, size, CV_8U);
	Mat gray_block_sum = Mat::zeros(size, size, CV_32S);
	Mat gray_block_count = Mat::zeros(size, size, CV_32S);

	double gray_block_rows_length = 1.0 * gray.rows / size;
	double gray_block_cols_length = 1.0 * gray.cols / size;

	// 該当範囲にあるピクセルの値を足し合わせる
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

	// ピクセルの値の平均を求める
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (gray_block_count.at<int>(y, x) != 0) {
				gray_block.at<unsigned char>(y, x) = gray_block_sum.at<int>(y, x) / gray_block_count.at<int>(y, x);
			}
		}
	}

	return gray_block;
}

/// <summary>
/// 二つの画像（グレースケールに限る）を並べて表示するための関数
/// </summary>
/// <param name="image_left">左側に表示したい画像</param>
/// <param name="image_right">右側に表示したい画像</param>
void showTwoImages(Mat image_left, Mat image_right) {
	Mat base(max(image_left.rows, image_right.rows), image_left.cols + image_right.cols, CV_8U);

	Mat roi_left(base, Rect(0, 0, image_left.cols, image_left.rows));
	image_left.copyTo(roi_left);

	Mat roi_right(base, Rect(image_left.cols, 0, image_right.cols, image_right.rows));
	image_right.copyTo(roi_right);

	imshow("gray block images", base);

}

/// <summary>
/// 二つのグレーブロック特徴を比較し、異なるピクセル数を数える関数
/// </summary>
/// <param name="image01_gray">一つ目のグレーブロック特徴</param>
/// <param name="image02_gray">二つ目のグレーブロック特徴</param>
/// <param name="new_color_threshold">新しく設定したい色の閾値（なければ負の値を入れる）</param>
/// <returns>異なるピクセル数を出力する</returns>
int countDiffPixels(Mat image01_gray, Mat image02_gray, int new_color_threshold) {
	int count_diff = 0;

	// 閾値の設定（省略可能にするため、引数の値がマイナスでなければ代入する）
	static int color_threshold = 2;
	if (new_color_threshold >= 0) {
		color_threshold = new_color_threshold;
	}

	for (int y = 0; y < image01_gray.rows; y++) {
		for (int x = 0; x < image01_gray.cols; x++) {
			if ((image01_gray.at<unsigned char>(y, x) < image02_gray.at<unsigned char>(y, x) - color_threshold) ||
				(image02_gray.at<unsigned char>(y, x) + color_threshold < image01_gray.at<unsigned char>(y, x))) {
				count_diff++;
			}
		}

	}

	return count_diff;

}

/// <summary>
/// 画像が重複画像かどうかを判断する関数
/// </summary>
/// <param name="count_diff_pixels">異なるピクセル数</param>
/// <param name="new_count_threshold">新しく設定したい異なるピクセル数の許容誤差（なければ負の値を入れる）</param>
/// <returns>重複画像であればtrue, 重複画像でなければfalse</returns>
bool isDuplicateImage(int count_diff_pixels, int new_count_threshold) {

	static int count_threshold = 5;
	if (new_count_threshold >= 0) {
		count_threshold = new_count_threshold;
	}

	bool ans;

	if (count_diff_pixels > count_threshold) {
		cout << count_diff_pixels << "個ピクセルが違うため，重複画像ではありません．" << endl;
		ans = false;
	}
	else {
		cout << count_diff_pixels << "個ピクセルが違いますが，許容誤差以内なので，重複画像です．" << endl;
		ans = true;
	}

	return ans;
}

/// <summary>
/// 編集履歴を分析する関数
/// </summary>
/// <param name="image01">一つ目の画像</param>
/// <param name="image02">二つ目の画像</param>
void analyseEditingHistory(Mat image01, Mat image02) {
	// ファイルの拡張子を調べる
	
	// ファイルの縦横の長さを調べる
	double vertical_rate = 1.0 * image01.cols / image02.cols;
	double horizontal_rate = 1.0 * image01.rows / image02.rows;
	cout << "画像01は、画像2の幅" << vertical_rate << "倍、";
	cout << "高さ" << horizontal_rate << "倍です．" << endl;

	// ファイルの色変換を調べる
	// RGBからHSVに変換する
	Mat image01_hsv;
	cvtColor(image01, image01_hsv, COLOR_BGR2HSV);
	Mat image02_hsv;
	cvtColor(image02, image02_hsv, COLOR_BGR2HSV);

	// セルを対応させるために画像サイズを小さいほうに合わせる
	resize(image02_hsv, image02_hsv, Size(), horizontal_rate, vertical_rate);

	// TODO: ウィンドウ名を指定して複数表示できるように
	// TODO; 型を受け取った画像に合わせる
	// showTwoImages(image01_hsv, image02_hsv);
	
	// cout << "画像01と画像02の間に行われた編集は" << "○○" << "です．" << endl;

}

/// <summary>
/// グレーブロック特徴の分割数nの値を変化させたときにコールバックする関数
/// </summary>
/// <param name="size">新しく設定されるnの値</param>
/// <param name="userdata">ユーザデータ</param>
void changeNValue(int size, void* userdata) {
	cout << "現在のn: " << size << endl;

	if (size > 0) {
		image01_gray = extractGrayBlock(image01, size);
		image02_gray = extractGrayBlock(image02, size);
	}
	else {
		image01_gray = extractGrayBlock(image01, 1);
		image02_gray = extractGrayBlock(image02, 1);
	}

	int count = size * size * 0.01;
	int count_diff = countDiffPixels(image01_gray, image02_gray, -1);

	if (isDuplicateImage(count_diff, count)) {
		analyseEditingHistory(image01, image02);
	}

	showTwoImages(image01_gray, image02_gray);
}

/// <summary>
/// 色の許容誤差を変更させたときにコールバックする関数
/// </summary>
/// <param name="color">新しい色の許容誤差の値</param>
/// <param name="userdata">ユーザデータ</param>
void changeColorValue(int color, void* userdata) {
	cout << "現在の色の許容誤差: " << color << endl;

	int count_diff = countDiffPixels(image01_gray, image02_gray, color);
	
	if (isDuplicateImage(count_diff, -1)) {
		analyseEditingHistory(image01, image02);
	}
}

/// <summary>
/// 異なるピクセル数の許容誤差を変化させたときにコールバックする関数
/// </summary>
/// <param name="count">異なるピクセル数の許容誤差</param>
/// <param name="userdata">ユーザデータ</param>
void changeCountValue(int count, void* userdata) {
	cout << "現在のピクセル数の許容誤差: " << count << endl;

	// TODO: count_diffを計算しなくても良いような実装にする
	int count_diff = countDiffPixels(image01_gray, image02_gray, -1);

	if (isDuplicateImage(count_diff, count)) {
		analyseEditingHistory(image01, image02);
	}
}

/// <summary>
/// 二つの画像から最も小さい辺の長さを求める関数
/// </summary>
/// <param name="image01">一つ目の画像</param>
/// <param name="image02">二つ目の画像</param>
/// <returns></returns>
int minTwoImagesEdgs(Mat image01, Mat image02) {

	int image01_min_edge = min(image01.cols, image01.rows);
	int image02_min_edge = min(image02.cols, image02.rows);
	int min_edge = min(image01_min_edge, image02_min_edge);

	return min_edge;
}
