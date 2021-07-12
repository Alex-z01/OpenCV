#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Basic CV Image Functions ///////////////

void main()
{
	string path = "Resources/me.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDilated, imgEroded;

	cvtColor(img, imgGray, COLOR_BGR2GRAY); //RGB to grayscale
	GaussianBlur(img, imgBlur, Size(3, 3), 3, 0); // Blur
	Canny(imgBlur, imgCanny, 25, 75); //Detect edges

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); //Creates kernel for dilation greater number more dilation
	dilate(imgCanny, imgDilated, kernel);
	erode(imgDilated, imgEroded, kernel);

	imshow("Image", img);
	imshow("Image Gray", imgGray);
	imshow("Image Blur", imgBlur);
	imshow("Image Canny", imgCanny);
	imshow("Image Dilated", imgDilated);
	imshow("Image Eroded", imgEroded);
	waitKey(0);
}


