#ifndef MAIN_H
#define MAIN_H

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

class image_and_capture{
	private:
		Mat image; // capture image
		Mat roi; // set the roi for counting circles
		vector <Vec3f> circles; // for using HoughCircles
		Mat images[3]; // for using histogram and similarlity
		Mat histograms[3];
		Mat gray;
		double per;
		double similar;
	public:
		VideoCapture capture; // usb video camera
		image_and_capture() : image(), roi() ,capture(0), per(0), similar(0)
		{
			circles.reserve(3);
		}
		image_and_capture(char * name) : roi(), capture(0), per(0), similar(0)
		{
			imread(name);
			circles.reserve(3);
		}
		image_and_capture(char * name, int num) : roi(), capture(num), per(0), similar(0) 
		{
			imread(name);
			circles.reserve(3);			
		}
		int capture_3min(void); // 
		bool capture_30sec(void);
		void cutting_image(Mat image, Rect rec );
		double percent(Mat image); // change image to HSV and extract yellow color
		void calc_histogram(Mat image, int num); // make two dimension histogram H(hue) S(saturation
		double check_similarity(Mat source, Mat compare); // find the value that similarity
		int counting_circle(Mat image); // find circles at roi 
		void video_capture(void);
		void check_percent(void);
};

#endif
