#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Importing Images ///////////////

void main()
{
	string path = "Resources/test.png";
	Mat img = imread(path);
	imshow("Image", img);
	waitKey(0);
}


//////////// Importing Videos ///////////////

void main()
{
	string path = "Resources/test_video.mp4";
	VideoCapture video(path);
	Mat img;

	while (true)
	{
		video.read(img);
		imshow("Image", img);
		waitKey(20);
	}
}


//////////// Webcam ///////////////

void main()
{

	VideoCapture video(0);
	Mat img;

	while (true)
	{
		video.read(img);
		imshow("Image", img);
		waitKey(1);
	}
}