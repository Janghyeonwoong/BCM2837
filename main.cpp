#include <iostream>
#include <opencv2/opencv.hpp>
#include "base.h"
#include "pwm.h"
#include "seg.h"
#include "main.h"

using namespace std;
using namespace cv;

bool stop = 0;

const int MOTOR = 0;
const int STOP = 1;

int main(void)
{
	int fd = open( "/dev/mem", O_RDWR | O_SYNC);
	if(fd < 0)
	{
		perror("There is no file that name of /dev/mem\n");
		exit(1);
	}

	init(fd);

	if(close(fd) < 0 )
	{
		perror("couldn't close /dev/mem file descriptor\n");
		exit(1);
	}
	reset();
	usleep(50);
	setup_switch();
	setup_7seg();
	setup_pwm();
	flash_off();
//	---------------------- base setup ----------------------------
	int gpio17 = 17;
	int gpio27 = 27;	
	pthread_t tid[5];
	int branch(0);
	float target = 60;

	image_and_capture cap;
	pthread_create(&tid[0], NULL, use_7seg, NULL);
	pthread_create(&tid[1], NULL, check_falling_edge_up, (void *)&gpio17);
	pthread_create(&tid[2], NULL, check_falling_edge_down, (void *)&gpio27);

	branch = cap.capture_3min();
	switch(branch)
	{
		case MOTOR : pthread_create(&tid[3], NULL, pid_control, (void *)&target);
					 cap.video_capture();
					 pthread_join(&tid[3], NULL);
					 target = 0;
					 pthread_create(&tid[4], NULL, pid_control, (void *)&target);
					 pthread_join(&tid[4], NULL);
					 target = 60;
					 break;
		default : break;


	}


	
	return 0;
}

int image_and_capture::capture_3min(void)
{

	while (stop == false)
	{
		capture.release();

		flash_on();
		capture.open(0);
		flash_off();
		capture >> image;
		per = percent(image);
		if (per > 70)
		{
			if (capture_30sec())
				return MOTOR;
		}
		sleep(10);
		
	}
	return STOP;

}
bool image_and_capture::capture_30sec(void)
{

	calc_histogram(image, 0);
	for (int i = 0; i<3; i++)
	{
		sleep(5);
		capture.release();
		flash_on();
		capture.open(0);
		flash_off();
	
		capture >> images[i];

		calc_histogram(images[i], i + 1);
		similar = check_similarity(histograms[0], histograms[i + 1]);
		if (similar < 90)
		{
			cout << "similarity :" << similar << endl;
			return false;
		}
		cout << "similar :" << similar << endl;
	}
	return true;
}
void image_and_capture::cutting_image(Mat image, Rect rec = Rect(0, 480 - 55, 120, 54))
{
	roi = image(rec);

}
double image_and_capture::percent(Mat image)
{
	Mat hsv;
	cvtColor(image, hsv, COLOR_BGR2HSV);

	inRange(hsv, Scalar(30 / 2, 30, 30), Scalar(80 / 2, 255, 255), hsv);

	unsigned int count = 0;
	for (int i = 0; i < hsv.rows; i++)
	{
		for (int j = 0; j < hsv.cols; j++)
		{
			if (hsv.at<uchar>(i, j) == 0)
				count++;
		}
	}

	double per;
	per = (double)count / (double)image.rows / (double)image.cols * 100;
	cout << "사진의 %는 " << (double)per << endl;
	return per;
}

void image_and_capture::calc_histogram(Mat image, int num)
{
	Mat hsv;
	Mat hist;
	static Vec3i bins(30, 42, 0);
	static Vec3f range(180, 256, 0);

	cvtColor(image, hsv, CV_BGR2HSV);

	static int channels[] = { 0 ,1 };
	static int histSize[] = { bins[0], bins[1] };

	static float range1[] = { 0 , range[0] };
	static float range2[] = { 0 , range[1] };

	const float*  ranges[] = { range1, range2 };

	calcHist(&hsv, 1, channels, Mat(), hist, 2, histSize, ranges);

	normalize(hist, hist, 0, 1, NORM_MINMAX);
	histograms[num] = hist;
}

double image_and_capture::check_similarity(Mat source, Mat compare)
{
	double value = compareHist(source, compare, CV_COMP_CORREL) * 100;
	return value;
}

int image_and_capture::counting_circle(Mat image)
{
	cvtColor(image, image, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(9, 9), 2, 2);

	HoughCircles(image, this->circles, CV_HOUGH_GRADIENT, 1, 100, 100, 40, 0, 100);
	STOP = (this->circles).size();
	waitKey(50);
	return (this->circles).size();
}

void image_and_capture::video_capture(void)
{
	int i(0);
	circles.clear();
	flash_on();
	while (i == 0)
	{		
		capture >> image;
		cutting_image(image);
		i = counting_circle(roi);
		cout << "원의 개수는 : " << i << endl;
	}
	flash_off();

}
