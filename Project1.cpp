#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Project 1 ///////////////

Mat img;
vector<vector<int>> newPoints;
int hmin, smin, vmin;
int hmax, smax, vmax;

// hmin, hmax, smin, smax, vmin, vmax
vector<vector<int>> myColors { 
	{54,85,13,153,0,255}, // Green
	{94,110,156,255,120,255}, // Blue
	};

vector<Scalar> myColorValues{ {0, 255, 0}, // Green
	{255, 0, 0}, // Blue
	};


Point getContours(Mat imgDilated)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDilated, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(drawImg, contours, -1, Scalar(0, 0, 255), 2);

	vector<vector<Point>> conPoly(contours.size()); //Has corners to tell what kind of shape
	vector<Rect> boundRect(contours.size());
	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		//cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			//cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, conPoly, i, Scalar(0, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
		}
	}
	return myPoint;
}


vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;

	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][2], myColors[i][4]);
		Scalar upper(myColors[i][1], myColors[i][3], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point myPoint = getContours(mask);

		if (myPoint.x != 0 && myPoint.y != 0)
		{
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}

	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> colors)
{
	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, colors[newPoints[i][2]], FILLED);
	}
}

void main()
{
	VideoCapture video(0);
	char choice;

	while (true)
	{
		video.read(img);
		flip(img, img, 1);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);

		imshow("Image", img);
		waitKey(1);

	}
}