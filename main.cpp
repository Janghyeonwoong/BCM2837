#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void info(Mat mat)
{
	cout << "this image is " << mat.rows << "x" << mat.cols << endl;
	return;
}

int main()
{

	Mat gray;
	//Canny(gray, gray, 10, 10*3, 3);
	//namedWindow("blur", CV_WINDOW_AUTOSIZE);
	//imshow("blur", gray);
	vector<Vec3f>circles;
	
	
	VideoCapture vc(0);

	if (!vc.isOpened()) return 0; // ¿¿¿¿

	Mat image;
	Mat roi;
	Point temp_circle;
	int temp_radius;
	int recurse(0);
	while (1) {
		vc >> image;
		circles.clear();
		if (image.empty()) break;
		Rect rec(0, 0, image.cols, 200);
		roi = image(rec);
		cvtColor(roi, roi, CV_BGR2GRAY);
		//GaussianBlur(gray, gray, Size(9, 9), 2, 2);
		HoughCircles(roi, circles, CV_HOUGH_GRADIENT, 1, 100, 100, 40, 0, 100);
		
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(image, center, 3, Scalar(0, 255, 0), -1);
			circle(image, center, radius, Scalar(0, 0, 255), 3, 8, 0);
			temp_circle = center;
			temp_radius = radius;
		}

		cv::imshow("cam", image);

		if (cv::waitKey(20) == 27) break; //ESC
		std::cout << circles.size() << endl;
		

									  //Sharpen(img, output);       //¿¿¿ ¿¿ ¿¿¿¿ ¿¿
									  //		imshow("output", output);

									  //filter2D(img, output, img.depth(), kern); //¿¿¿ ¿¿ ¿¿¿¿ ¿¿
									  //	imshow("output2", output);
	}
	info(image);
	cv::destroyAllWindows();
	return 0;
	

}
