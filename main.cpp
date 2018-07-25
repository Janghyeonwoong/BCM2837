#include <iostream>
#include <opencv2/opencv.hpp>
#include "base.h"
#include "pwm.h"
#include "seg.h"
#include "main.h"

extern "C" {
#include <pthread.h>
}
using namespace std;
using namespace cv;

bool stop = 0;

enum tree { 
MOTOR,
STOP
}branch;	


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
	setup_pwm();
	setup_switch();
	setup_7seg();

//	---------------------- base setup ----------------------------
	const int gpio17 = 17;
	const int gpio27 = 27;	
	pthread_t tid[5];
	image_and_capture cap;
	pthread_create(tid[0], NULL, use_7seg, NULL);
	pthread_create(tid[1], NULL, check_falling_edge_down, &gpio17);
	pthread_create(tid[2], NULL, check_falling_edge_down, &gpio27);

	branch = cap.capture_3min();
	switch(branch)
	{
		case MOTOR : change_pwm(pwm , 80);
				     sleep(3);
					 cap.video_capture();
					 change_pwm(pwm , 0 );
					 break;
		default : break;


	}


	
	return 0;
}

class image_and_capture{
	private:
		Mat image; // capture image
		Mat roi; // set the roi for counting circles
		vector <Vec3f> circles; // for using HoughCircles
		Mat images[3]; // for using histogram and similarlity
		vector <Mat> histograms;
		Mat gray;
		double per;
		double similar;
	public:
		VideoCapture capture; // usb video camera
		image_and_capture() : image(), roi() ,caputre(0), per(0), similar(0)
		{
			circles.reserve(3);
			histograms.reserve(4);
		}
		image_and_capture(char * name) : roi(), capture(0), per(0), similar(0)
		{
			imread(name, image);
			circles.reserve(3);
			histograms.reserve(4);
		}
		image_and_capture(char * name, int num) : roi(), capture(num), per(0), similar(0) 
		{
			imread(name, image);
			circles.reserve(3);			
			histograms.reserve(4);
		}
		int capture_3min(void); // 
		bool capture_30sec(void);
		void cutting_image(Mat image, Rect(0, 480 - 55, 120, 54) );
		double percent(Mat image); // change image to HSV and extract yellow color
		void calc_histogram(Mat image); // make two dimension histogram H(hue) S(saturation
		double check_similarity(Mat source, Mat compare); // find the value that similarity
		int counting_circle(Mat image); // find circles at roi 
		void video_capture(void);
}
/* * you have to make enum fucntion for switch-case 
   *
   */ 

int image_and_cpature::capture_3min(void)
	{
		while(stop == false)
		{
			vc >> image;
			per = percent(image);
			if(per > double(seg_value * 10) )
			{
				if(capture_30sec())
					return MOTOR;
			}
			sleep(60*3);
		}
		return STOP;

	}
bool image_and_capture::capture_30sec(void)
	{
		histograms.clear();
		calc_histogram(image);
		for(int i=0; i<3 ; i++)
		{
			sleep(30);
			vc >> images[i];
			calc_histogram(images[i]);
			similar = check_similarity(histograms[0] , histograms[i+1]);
			if(similar < 90)
			{
				cout << "similarity < 90" << endl;
				return false;
			}
		}
		return true;
	}
void image_and_capture::cutting_image(Mat image, Rect rec = Rect(0, 480 - 55, 120, 54))
{
	roi = image(rec);

}
double image_and_capture::percent(Mat image)
{
	cvtColor(image, image, COLOR_BGR2HSV);
	inRange(image, Scalar(30 / 2, 30, 30), Scalar(80 / 2, 255, 255), image);
	unsigned int count = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.at<uchar>(i, j) == 0)
				count++;
		}
	}
	double per;
	per = (double)count / (double)image.rows / (double)image.cols * 100;
	cout << (double)per << endl;
	return per;
}

void image_and_capture::calc_histogram(Mat image)
{
	Mat hsv;
	Mat hist;
	static Vec3i bins(30, 42, 0);
	static Vec3f range(180, 256, 0);

	cvtColor(image, Hsv, CV_BGR2HSV);

	static int channels[] = { 0 ,1 };
	static int histSize[] = { bins[0], bins[1]};

	static float range1[] = { 0 , range[0] };
	static float range2[] = { 0 , range[1] };
	
	const float*  ranges[] = { range1, range2};

	calcHist(hsv, 1, channels, Mat(), hist, 2, histSize, ranges);
	normalize(hist, hist, 0, 1, NORM_MINMAX);
	(this->histograms).pushback(hist);
}

double image_and_capture::check_similarity(Mat source, Mat compare)
{
	double value = compareHist(source, compare, CV_COMP_CORREL);
	return value;
}

int image_and_capture::counting_circle(Mat image)
{
	color(image, image, CV_BGR2GRAY);
	//GaussianBlur(gray, gray, Size(9, 9), 2, 2);
	HoughCircles(image, this->circles, CV_HOUGH_GRADIENT, 1, 100, 100, 40, 0, 100);
	return (thie->circles).size();
}

void image_and_capture::video_capture(void)
{
	int i(0);
	circles.clear();
	while(i == 0)
	{
		capture >> image;
		cutting_image(image);
		i = counting_circle(roi);
	}
	

}
