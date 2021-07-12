#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Drawing Shapes and Text ///////////////

void main()
{
	// Blank image
	Mat img(512, 512, CV_8UC3, Scalar(255, 255, 255));

	circle(img, Point(256, 256), 155, Scalar(0, 0, 0), 5);
	circle(img, Point(256, 256), 153, Scalar(255, 0, 150), FILLED); 

	rectangle(img, Point(130, 226), Point(382, 286), Scalar(255, 255, 255), FILLED);

	line(img, Point(130, 296), Point(382, 296), Scalar(255, 255, 255), 2);

	putText(img, "Absolute", Point(137, 262), FONT_HERSHEY_DUPLEX, 1.2, Scalar(255, 0, 150), 2);

	imshow("Image", img);

	waitKey(0);
}


