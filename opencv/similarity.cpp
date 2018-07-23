#if 0
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

double calc_similarity(Mat source, Mat compare)
{
	double value = compareHist(source, compare, CV_COMP_CORREL);
	return value;
}

void info(Mat mat)
{
	cout << "this image is " << mat.rows << "x" << mat.cols << endl;
	return;
}
void calc_Histo(const Mat& img, Mat& hist, Vec3i bins, Vec3f range, int _dims)
{
	int dims = (_dims <= 0) ? img.channels() : _dims;
	int channels[] = { 0 ,1 };
	int histSize[] = { bins[0], bins[1]};

	float range1[] = { 0 , range[0] };
	float range2[] = { 0 , range[1] };
	
	const float*  ranges[] = { range1, range2};

	calcHist(&img, 1, channels, Mat(), hist, dims, histSize, ranges);
	normalize(hist, hist, 0, 1, NORM_MINMAX);

}

Mat draw_histo(Mat hist)
{
	if (hist.dims != 2) {
		cout << "히스토그램이 2차원 데이터가 아님니다." << endl;
		exit(1);
	}
	float ratio_value = 512;
	float ratio_hue = 180.f / hist.rows;			// 색상 스케일 비율
	float ratio_sat = 256.f / hist.cols;			// 채도 스케일 비율

	Mat graph(hist.size(), CV_32FC3);
	for (int i = 0; i < hist.rows; i++) {
		for (int j = 0; j < hist.cols; j++)
		{
			float value = hist.at<float>(i, j) * ratio_value;
			float hue = i * ratio_hue;
			float sat = j * ratio_sat;
			graph.at<Vec3f>(i, j) = Vec3f(hue, sat, value);
		}
	}

	graph.convertTo(graph, CV_8U);
	cvtColor(graph, graph, CV_HSV2BGR);
	resize(graph, graph, Size(0, 0), 10, 10, INTER_NEAREST);

	return graph;
}

int main()
{
	Mat Hsv1, Hsv2;
	Mat hist1, hist2;
	Mat hist_img1, hist_img2;

	Mat image1 = imread("mouse1.jpg", IMREAD_COLOR);
	Mat image2 = imread("mouse2.jpg", IMREAD_COLOR);

	Vec3i bins(30, 42, 0);
	Vec3f ranges(180, 256, 0);

	double value;
	resize(image1, image1, Size(144 * 3, 256 * 3), 0, 0, CV_INTER_LINEAR);
	resize(image2, image2, Size(144 * 3, 256 * 3), 0, 0, CV_INTER_LINEAR);

	cvtColor(image1, Hsv1, CV_BGR2HSV);
	cvtColor(image2, Hsv2, CV_BGR2HSV);


	calc_Histo(Hsv1, hist1, bins, ranges, 2);
	calc_Histo(Hsv2, hist2, bins, ranges, 2);

	value = calc_similarity(hist1, hist2);

	hist_img1 = draw_histo(hist1);
	hist_img2 = draw_histo(hist2);
	
	imshow("image1", image1);
	imshow("image2", image2);
	imshow("hist1", hist_img1);
	imshow("hist2", hist_img2);
	cout << "유사도 : " << (double)value << endl;

	waitKey(0);
	info(image1);

	return 0;

}
#endif
