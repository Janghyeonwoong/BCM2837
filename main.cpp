#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;

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

void onMouse(int event, int x, int y, int flags, void* param) {

	cv::Mat *im = reinterpret_cast<cv::Mat*>(param);

	switch (event) {	// dispatch the event

	case cv::EVENT_LBUTTONDOWN: // mouse button down event

								// display pixel value at (x,y)
		std::cout << "at (" << x << "," << y << ") value is: "
			<< "색상 : " << static_cast<int>(im->at<Vec3b>(y, x)[0]) <<
			" 채도 : " << static_cast<int>(im->at<Vec3b>(y, x)[0]) <<
			" 명도 : " << static_cast<int>(im->at<Vec3b>(y,x)[0]) << std::endl;
		break;
	}
}
#if 1
int main(void)
{
	unsigned int count = 0;
	//info(image);
	Mat image = imread("pee3.jpg",CV_LOAD_IMAGE_ANYCOLOR);
	info(image);
	std::cout << "This image has "
		<< image.channels() << " channel(s)" << std::endl;
	//checkfile(image);

	//namedWindow("Original Image");
	imshow("Output image", image);
	Mat cvt;
	cvtColor(image, cvt, COLOR_BGR2HSV);
	Mat cvt2;
	//imshow("cvtimage", cvt);
	inRange(cvt, Scalar(30/2, 15, 15), Scalar(80/2, 255, 255), cvt2);
	imshow("trasnformer", cvt2);

	std::cout << "cvt2 "
		<< cvt2.channels() << " channel(s)" << std::endl;

	
	for (int i = 0; i < cvt2.rows; i++)
	{
		for (int j = 0; j < cvt2.cols; j++)
		{
			if (cvt2.at<uchar>(i, j) == 0)
			{
				count++;
				image.at<Vec3b>(i, j)[0] = 0;
				image.at<Vec3b>(i, j)[1] = 0;
				image.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}
	imshow("retransform", image);
	long double per;
	per = (long double)count / (long double)cvt2.rows / (long double)cvt2.cols * 100;
	cout << (long double)per << endl;
	
/*	
	Mat result;
	flip(image, image, 1);
	imshow("Output Image", image);
	//imwrite("Output.bmp", result);
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			image.at<Vec3b>(i, j)[0] = 200;
			image.at<Vec3b>(i, j)[1] = 200;
			image.at<Vec3b>(i, j)[2] = 30;

		}
	}
	imshow("Original Image", image);
	*/
	//cv::setMouseCallback("cvtimage", onMouse, reinterpret_cast<void*>(&image));

	waitKey(-1);


										 // set the mouse callback for this image
	return 0;
}
#endif