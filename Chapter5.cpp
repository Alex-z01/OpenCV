#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//////////// Warping ///////////////

float w = 250;
float h = 350;
Mat matrixK, matrixQ, matrix9, matrixJ, imgK, imgQ, img9, imgJ;

void main()
{
	string path = "Resources/cards.jpg";
	Mat img = imread(path);

	Point2f src[4] = { {529, 142} , {771, 190}, {405, 395}, {674, 457} };
	Point2f queen[4] = { {64, 324}, {337, 279}, {90, 635}, {402, 573} };
	Point2f nine[4] = { {743, 384}, {1021, 438}, {647, 707}, {965, 783} };
	Point2f jack[4] = { {777, 108}, {1017, 83}, {841, 357}, {1115, 331} };
	Point2f dest[4] = { {0.0f, 0.0f} , {w, 0.0f}, {0.0f, h}, {w, h} };

	matrixK = getPerspectiveTransform(src, dest);
	matrixQ = getPerspectiveTransform(queen, dest);
	matrix9 = getPerspectiveTransform(nine, dest);
	matrixJ = getPerspectiveTransform(jack, dest);

	warpPerspective(img, imgK, matrixK, Point(w, h));
	warpPerspective(img, imgQ, matrixQ, Point(w, h));
	warpPerspective(img, img9, matrix9, Point(w, h));
	warpPerspective(img, imgJ, matrixJ, Point(w, h));

	for (int i = 0; i < 4; i++)
	{
		circle(img, src[i], 10, Scalar(0, 0, 255), 2);
		circle(img, queen[i], 10, Scalar(0, 0, 255), 2);
		circle(img, nine[i], 10, Scalar(0, 0, 255), 2);
		circle(img, jack[i], 10, Scalar(0, 0, 255), 2);
	}

	imshow("Image", img);
	imshow("Card", imgK);
	imshow("Queen", imgQ);
	imshow("Nine", img9);
	imshow("Jack", imgJ);

	waitKey(0);
}


