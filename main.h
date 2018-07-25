#ifndef MAIN_H
#define MAIN_H

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

#endif
