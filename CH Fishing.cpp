#include <Windows.h>
#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

Mat target, target2;
LPCWSTR wName = L"BlueStacks";
HWND hWND = FindWindow(NULL, wName); //Find the window
HWND desktop = GetDesktopWindow();
int hmin, smin, vmin, hmax, smax, vmax;
bool go = false;
bool purpling = false;
vector<Vec3f> circles;

//hmin smin vmin hmax smax vmax
vector<vector<int>> myColors{
    //{85, 159, 118, 106, 255, 129} // Regular fishing spot
    {0, 0, 95, 50, 75, 130}
};

vector<Scalar> boxColors{
    {255, 255, 255}, // White
    {0, 0, 255} // Red
};

void ManualKeys();
void getContours(Mat img, Mat draw, int col);
Mat getMat(HWND hWND);
void Begin();
void FindPurple();
void LeftClick();
void MouseMove(int x, int y);


vector<vector<int>> points = { {1617,345},{1623,476} };

void ManualKeys()
{
    if (GetAsyncKeyState(VK_NUMPAD0)) {
        return;
    }
    if (GetAsyncKeyState(VK_NUMPAD1)) {
        Sleep(150);
        if (go == false) {
            go = true;
            cout << "ON" << endl;
        }
        else {
            go = false;
            cout << "OFF" << endl;
        }
        Begin();
    }
    if (GetAsyncKeyState(VK_NUMPAD2)) {
        Sleep(150);
        POINT p;
        if (GetCursorPos(&p))
        {
            cout << p.x << " | " << p.y << endl;           
        }
    }
    if (GetAsyncKeyState(VK_NUMPAD3)) {
        Sleep(150);
        purpling = true;
        FindPurple();
    }
}

void getContours(Mat img, Mat drawImg, int color)
{

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //drawContours(drawImg, contours, -1, Scalar(0, 0, 255), 2);

    vector<vector<Point>> conPoly(contours.size()); //Has corners to tell what kind of shape
    vector<Rect> boundRect(contours.size());

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);

        if (area > 1000 && area < 2200)
        {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            //drawContours(drawImg, conPoly, i, Scalar(255, 0, 150), 2);
            boundRect[i] = boundingRect(conPoly[i]);
            rectangle(drawImg, boundRect[i].tl(), boundRect[i].br(), boxColors[color], 4); // Draw bounding box
            Point center = Point(boundRect[i].x+50, boundRect[i].y);
            MouseMove(center.x, center.y);
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
    int width = rect.right - rect.left;

    HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height); //Create a bitmap of screenshot

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

void FindPurple() {

    //vector<int> purple = { 120, 70, 0, 132, 157, 255 };
    go = false;
    bool probOOM = false;
    int loop = 0;
    Sleep(4000);
    while (purpling)
    {
        if (loop < 50)
        {
            loop++;
        }
        ManualKeys();
        Mat imgHSV, purpMask;
        target2 = getMat(hWND);

        cvtColor(target2, imgHSV, COLOR_BGR2HSV);

        Scalar lower(120, 70, 0); //Hue minimum, saturation minimum, value minimum
        Scalar upper(132, 157, 255); //Hue max, saturation max, value max
        inRange(imgHSV, lower, upper, purpMask);
        imshow("purpleMask", purpMask);
        int pixels = countNonZero(purpMask);
        cout << "PIXELS: " << to_string(pixels) << endl;
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(purpMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<vector<Point>> conPoly(contours.size()); //Has corners to tell what kind of shape
        vector<Rect> boundRect(contours.size());

        for (int i = 0; i < contours.size(); i++)
        {
            int area = contourArea(contours[i]);

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            boundRect[i] = boundingRect(conPoly[i]);

            putText(target, to_string(area), boundRect[i].tl(), FONT_HERSHEY_DUPLEX, 1.2, Scalar(255, 0, 150), 2);
            if (area > 800) {
                cout << "MANA GOOD" << endl;
                probOOM = false;
            }
            else if (area < 750 && area > 475) {
                probOOM = false;
                cout << "LOW MANA" << endl;
                for (int i = 0; i < 3; i++)
                {
                    INPUT Input = { 0 };
                    Input.type = INPUT_KEYBOARD;
                    Input.ki.wScan = 0;
                    Input.ki.time = 0;
                    Input.ki.dwExtraInfo = 0;

                    Input.ki.wVk = 0x32;
                    Input.ki.dwFlags = 0;
                    SendInput(1, &Input, sizeof(INPUT));

                    Input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
                    SendInput(1, &Input, sizeof(INPUT));
                }
            }
            else if (area < 250 && area > 100) {
                cout << "TOO LOW" << endl;
                purpling = false;
                probOOM = true;
            }
            else if (area < 100 && pixels < 10 && loop > 40) {
                printf("DONE FISHING\n");
                purpling = false;
                probOOM = false;
            }
        }

        waitKey(25);
    }

    if (!purpling && !probOOM) {
        printf("RESTART\n");
        Sleep(150);
        if (go == false) {
            go = true;
            cout << "ON" << endl;
        }
        else {
            go = false;
            cout << "OFF" << endl;
        }
        Begin();
    }
    if (!purpling && probOOM)
    {
        printf("RESTART LONG\n");
        Sleep(360000);
        go = true;
        Begin();
    }
}

// LeftClick function
void LeftClick()
{
    for (int i = 0; i < 2; i++)
    {
        INPUT Input = { 0 };
        // left down
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        ::SendInput(1, &Input, sizeof(INPUT));

        // left up
        ::ZeroMemory(&Input, sizeof(INPUT));
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        ::SendInput(1, &Input, sizeof(INPUT));
        Sleep(50);
    }
    purpling = true;
    Sleep(150);
    FindPurple();
}

// MouseMove function
void MouseMove(int x, int y)
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x * (65535.0f / fScreenWidth);
    double fy = y * (65535.0f / fScreenHeight);
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));

    LeftClick();
}


void Begin()
{
    /*
    namedWindow("Trackbars", (640, 200));
    createTrackbar("Hue Min", "Trackbars", &hmin, 179);
    createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    createTrackbar("Sat Min", "Trackbars", &smin, 255);
    createTrackbar("Sat Max", "Trackbars", &smax, 255);
    createTrackbar("Val Min", "Trackbars", &vmin, 255);
    createTrackbar("Val Max", "Trackbars", &vmax, 255);
    */

    while (go) {
        ManualKeys();
        target = getMat(hWND);
        Mat imgHSV, mask, canny;
        //GaussianBlur(target, target, Size(3, 3), 3, 0);
        cvtColor(target, imgHSV, COLOR_BGR2HSV);

        /*
        Scalar lower(hmin, smin, vmin); //Hue minimum, saturation minimum, value minimum
        Scalar upper(hmax, smax, vmax); //Hue max, saturation max, value max
        inRange(imgHSV, lower, upper, mask);
        */

        for (int i = 0; i < myColors.size(); i++)
        {
            Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]); //Hue minimum, saturation minimum, value minimum
            Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]); //Hue max, saturation max, value max
            inRange(imgHSV, lower, upper, mask);
            getContours(mask, target, i);
        }

        imshow("Target", target);

        waitKey(10);
    }
}

int main()
{
    namedWindow("Target", WINDOW_AUTOSIZE);
    while (true) {
        ManualKeys();
    }
}