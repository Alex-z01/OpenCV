#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

#include <vector>
#include <Windows.h>

using namespace std;
using namespace cv;

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

		if (area > 100)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			drawContours(drawImg, conPoly, i, Scalar(0, 0, 255), 2);
			boundRect[i] = boundingRect(conPoly[i]);

			//rectangle(drawImg, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5); // Draw bounding box

		}
	}
}

Mat getMat(HWND hWND)
{
	HDC deviceContext = GetDC(hWND); //HDC is handle device context, basically a canvas for GDI
	HDC memoryDC = CreateCompatibleDC(deviceContext);
	RECT rect; //Initialize rectangle

	//Get dimensions of window into rect
	GetWindowRect(hWND, &rect);
	int height = rect.bottom - rect.top;
	int width =  rect.right - rect.left;

	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height); //Create a bitmap to screenshot

	SelectObject(memoryDC, bitmap); // selects an object into the specified device context(DC) HDC comptatible DC into bitmap
	
	//copy data into bitmap
	BitBlt(memoryDC, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY); 
	//BitBlt performs a bit-block transfer of the color data corresponding to a rectangle 
	//of pixels from the specified source device context into a destination device context.

	//Setup the bitmap format using BITMAPINFOHEADER
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0; //because no compression
	bi.biXPelsPerMeter = 0; //irrelevant
	bi.biYPelsPerMeter = 0; //irrelevant
	bi.biClrUsed = 0; //irrelevant
	bi.biClrImportant = 0; //irrelevant 

	Mat mat = Mat(height, width, CV_8UC4); // 8 bit unsigned ints 4 Channels -> RGBA 

	//transform data and store into mat.data
	GetDIBits(memoryDC, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	//clean up!
	DeleteObject(bitmap);
	DeleteDC(memoryDC); //delete not release!
	ReleaseDC(hWND, deviceContext);

	return mat;
}

Mat mask, imgHSV, imgBB, imgCanny, imgBlank;
int hmin = 49, smin = 93, vmin = 0;
int hmax = 63, smax = 184, vmax = 255;

int main()
{
	LPCWSTR wName = L"Minecraft 1.17.1";
	HWND hWND = FindWindow(NULL, wName); //Find the window
	HWND desktop = GetDesktopWindow();
	namedWindow("output", WINDOW_NORMAL);

	namedWindow("Trackbars", (640, 200));
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);

	while (true)
	{
		Mat target = getMat(hWND);
		Mat bg;
		target.copyTo(bg);
		//cvtColor(target, target, COLOR_BGR2HSV);

		cvtColor(target, imgHSV, COLOR_BGR2HSV);

		Scalar lower(hmin, smin, vmin); //Hue minimum, saturation minimum, value minimum
		Scalar upper(hmax, smax, vmax); //Hue max, saturation max, value max
		inRange(imgHSV, lower, upper, mask);
		
		Canny(mask, imgCanny, 25, 75);
		getContours(imgCanny, target);
		
		//imshow("wtf", imgBB);
		imshow("output", target);
		//imshow("canny", imgCanny);

		//imshow("output", bg);
		waitKey(10);
	}
}