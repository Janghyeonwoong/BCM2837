#if 0
#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void info(Mat mat);
void checkfile(Mat img);
void filtering(Mat& image, Mat copy, int option);
void opening(Mat & image);
void closing(Mat &image);
long double percent(Mat image);
void  calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256)
{
	int		histSize[] = { bins };			// 히스토그램 계급개수
	float   range[] = { 0, (float)range_max };		// 히스토그램 범위
	int		channels[] = { 0 };				// 채널 목록
	int		dims = image.channels();;
	const float* ranges[] = { range };

	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges);
}

// hue 채널에 대한 색상 팔레트 행렬 생성
Mat  make_palatte(int rows)
{
	Mat hsv(rows, 1, CV_8UC3);
	for (int i = 0; i < rows; i++)
	{
		uchar hue = saturate_cast<uchar>((float)i / rows * 180);
		hsv.at<Vec3b>(i) = Vec3b(hue, 255, 255);
	}
	cvtColor(hsv, hsv, CV_HSV2BGR);
	return hsv;
}

void draw_histo_hue(Mat hist, Mat &hist_img, Size size = Size(256, 200))
{
	Mat hsv_palatte = make_palatte(hist.rows);

	hist_img = Mat(size, CV_8UC3, Scalar(255, 255, 255));
	float  bin = (float)hist_img.cols / hist.rows;
	normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX);

	for (int i = 0; i<hist.rows; i++)
	{
		float start_x = (i * bin);
		float  end_x = (i + 1) * bin;
		Point2f pt1(start_x, 0);
		Point2f pt2(end_x, hist.at <float>(i));


		Scalar color = hsv_palatte.at<Vec3b>(i);				// 색상팔레트 색지정
		if (pt2.y>0) rectangle(hist_img, pt1, pt2, color, -1);	// 팔레트 색 그리기	
	}
	flip(hist_img, hist_img, 0);
}


void info(Mat mat)
{
	cout << "this image is " << mat.rows << "x" << mat.cols << endl;
	return;
}
void checkfile(Mat img)
{
	if (img.empty())
	{
		cout << "There is not a image " << endl << "Please check the File plz" << endl;
		return;
	}
}
void filtering(Mat& image, Mat copy, int option)
{
	if (option == 1) // 침식
	{
		for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				if (image.at<uchar>(i, j) == 255)
				{
					if (image.at<uchar>(i - 1, j) == 0 || image.at<uchar>(i + 1, j) == 0 || image.at<uchar>(i, j - 1) == 0 || image.at<uchar>(i, j + 1) == 0
						|| image.at<uchar>(i - 1, j-1) == 0 || image.at<uchar>(i - 1, j + 1) == 0 || image.at<uchar>(i + 1, j - 1) == 0 || image.at<uchar>(i + 1, j + 1) == 0  
						)
						copy.at<uchar>(i, j) = 0;
				}
				else
					continue;

			}
		}
		for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				if (copy.at<uchar>(i, j) == 0)
					image.at<uchar>(i, j) = 0;
			}
		}
	}
	else // 팽창
	{
		for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				if (image.at<uchar>(i, j) == 0)
				{
					if (image.at<uchar>(i - 1, j) == 255 || image.at<uchar>(i + 1, j) == 255 || image.at<uchar>(i, j - 1) == 255 || image.at<uchar>(i, j + 1) == 255
						|| image.at<uchar>(i - 1, j - 1) == 255 || image.at<uchar>(i - 1, j + 1) == 255 || image.at<uchar>(i + 1, j - 1) == 255 || image.at<uchar>(i + 1, j + 1) == 255
						)
						copy.at<uchar>(i, j) = 255;

				}
				else
					continue;

			}
		}
		for (int i = 1; i < image.rows - 1; i++)
		{
			for (int j = 1; j < image.cols - 1; j++)
			{
				if (copy.at<uchar>(i, j) == 255)
					image.at<uchar>(i, j) = 255;
			}
		}
	}
}
void opening(Mat & image)
{
	int row, col;
	Mat temp(image.rows, image.cols, CV_8UC1, 122);
//	filtering(image, temp, 1); // 침식
	filtering(image, temp, 1); // 침식
	imshow("침식후에2", image);
//filtering(image, temp, 0); // 팽창
	filtering(image, temp, 0); // 팽창

	imshow("팽창후에", image);

}
void closing(Mat &image)
{
	Mat temp(image.rows, image.cols, CV_8UC1, 122);
	filtering(image, temp, 0); // 팽창
	filtering(image, temp, 1); // 침식
}
long double percent(Mat image)
{
	unsigned int count = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.at<uchar>(i, j) == 0)
				count++;
		}
	}
	long double per;
	per = (long double)count / (long double)image.rows / (long double)image.cols * 100;
	cout << (long double)per << endl;
	return per;
}
int main(void)
{
	unsigned int count = 0;
	Mat image = imread("pee2.jpg",CV_LOAD_IMAGE_ANYCOLOR);
	imshow("Output image", image);
	
	Mat cvt;
	cvtColor(image, cvt, COLOR_BGR2HSV);
	
	Mat cvt2;
	inRange(cvt, Scalar(30 / 2, 30, 30), Scalar(80 / 2, 255, 255), cvt2);
	imshow("trasnformer1", cvt2);
	percent(cvt2);

	inRange(cvt, Scalar(30 / 2, 30, 30), Scalar(80 / 2, 200, 200), cvt2);
	imshow("trasnformer2", cvt2);
	percent(cvt2);
	
	/* 히스토그램 
	Mat HSV_img, HSV_arr[3];
	cvtColor(image, HSV_img, CV_BGR2HSV);
	split(HSV_img, HSV_arr);

	Mat hue_hist, hue_hist_img;
	calc_Histo(HSV_arr[0], hue_hist, 18, 180);// Hue 채널 히스토그램 계산
	draw_histo_hue(hue_hist, hue_hist_img, Size(360, 200)); // 히스토그램 그래프

	imshow("image", image);
	imshow("Hue_hist_img", hue_hist_img);
	waitKey();
	return 0;
	*/

	/*
	percent(cvt2);
	closing(cvt2);
	imshow("mopology", cvt2);
	percent(cvt2);
	
	*/
	waitKey(0);
	
	return 0;
}

#endif