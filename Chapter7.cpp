#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Shape Detection ///////////////

Mat imgGray, imgBlur, imgCanny, imgDilated;

void getContours(Mat imgDilated, Mat drawImg)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDilated, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(drawImg, contours, -1, Scalar(0, 0, 255), 2);

	vector<vector<Point>> conPoly(contours.size()); //Has corners to tell what kind of shape
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		//cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			drawContours(drawImg, conPoly, i, Scalar(0, 0, 255), 2);
			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			//putText(drawImg, to_string(area), contours[i][0], FONT_HERSHEY_DUPLEX, .75, Scalar(0, 0, 0), 1);

			int objCor = (int)conPoly[i].size();

			if (objCor == 3) {objectType = "Triangle";}
			if (objCor == 4) 
			{
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				cout << aspRatio << endl;
				if (aspRatio > 0.95 && aspRatio < 1.05)
				{
					objectType = "Square";
				}else{ objectType = "Rectangle"; }
			}
			if (objCor > 4) {objectType = "Circle";}

			rectangle(drawImg, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5); // Draw bounding box
			putText(drawImg, objectType, {boundRect[i].x, boundRect[i].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 1); // Draw text
		}
	}
}

void main()
{
	string path = "Resources/shapes2.png";
	Mat img = imread(path);
	
	// Preprocessing
	cvtColor(img, imgGray, COLOR_BGR2GRAY); //RGB to grayscale
	GaussianBlur(img, imgBlur, Size(3, 3), 3, 0); // Blur
	Canny(imgBlur, imgCanny, 25, 75); //Detect edges
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); //Creates kernel for dilation greater number more dilation
	dilate(imgCanny, imgDilated, kernel);

	imshow("Image", img);

	getContours(imgDilated, img);

	imshow("Processed Image", img);

	waitKey(0);
}
