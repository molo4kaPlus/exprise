#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <winuser.h>
#include <ctime>
#include <thread>

using namespace std;
using namespace cv;

#pragma warning(disable : 4996)

BITMAPINFOHEADER createBitmapHeader(int scan_radius)
{
    BITMAPINFOHEADER  bi;

    // create a bitmap
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = scan_radius;
    bi.biHeight = -scan_radius;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

Mat captureScreenMat(HWND hwnd, int scan_radius, int x, int y)
{
    Mat src;

    // get handles to a device context (DC)
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // create mat object
    src.create(scan_radius, scan_radius, CV_8UC4);

    // create a bitmap
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, scan_radius, scan_radius);
    BITMAPINFOHEADER bi = createBitmapHeader(scan_radius);

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, scan_radius, scan_radius, hwindowDC, x/2 - scan_radius/2 , y/2 - scan_radius/2, scan_radius, scan_radius, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, scan_radius, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);                                                      //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

void get_pixels(HDC &hwindowCompatibleDC, HDC &hwindowDC, BITMAPINFOHEADER &bi, HBITMAP &hbwindow, Mat &src, int scan_radius, int x, int y)
{
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, scan_radius, scan_radius, hwindowDC, x / 2 - scan_radius / 2, y / 2 - scan_radius / 2, scan_radius, scan_radius, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, scan_radius, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);                                                      //copy from hwindowCompatibleDC to hbwindow

}

float diff(Vec3b C1, Vec3b C2)
{
    return sqrt((C1[0] - C2[0]) * (C1[0] - C2[0]) + (C1[1] - C2[1]) * (C1[1] - C2[1]) + (C1[2] - C2[2]) * (C1[2] - C2[2]));
}

void have_to_fire(Mat pixel_mat, int scan_radius, Vec3b color)
{
    for (int i = 0; i < scan_radius; i++) {
        for (int j = 0; j < scan_radius; j++) {
            if (diff(pixel_mat.at<Vec3b>(i, j), color) <= 50) {
                keybd_event(0x4A, 0, 0, 0); keybd_event(0x4A, 0, KEYEVENTF_KEYUP, 0); 
                return;
            }
        }
    }
}

bool validate_key(string key, string key2, tm* gmtm)
{
    if (gmtm->tm_mday <= 28 && gmtm->tm_mon == 1 && key == key2)
    {
        return true;
    }
    cout << "key invalid!" << endl;
    getchar();
    return false;
}

void revalidate_key(string key, string key2)
{
    time_t now = time(0);
    tm* gmtm = gmtime(&now);
    while (true)
    {
        Sleep(5000);
        if (!(gmtm->tm_mday <= 28 && gmtm->tm_mon == 1 && key == key2))
        {
            cout << "key invalid." << endl;
            getchar();
            exit(0);
        }
    }
}

int main()
{
    string key = "DAYLKZT<KB";
    tm valid_time;
    valid_time.tm_year = 2024;       // год окончания
    valid_time.tm_mon  = 2 - 1;      // месяц окончания
    valid_time.tm_mday = 28;         // день окончания
    valid_time.tm_hour = 0;          // час окончания
    int scan_radius = 8;             // Например: 100x100 (=100)
    int x = 1920;                    // Ширина           
    int y = 1080;                    // Высота
    int delta = 10;
    Vec3b color = {63, 254, 254};    // искомый цвет

    time_t now = time(0);
    tm* gmtm = gmtime(&now);
    char* dt = ctime(&now);
    dt = asctime(gmtm);

    string key2;
    //cout << "input key..." << endl;
    //cin >> key2;
    //if (!validate_key(key, key2, gmtm)) { getchar(); return 1; }


    HWND hwnd = GetDesktopWindow();
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    Mat pixel_matrix;
    pixel_matrix.create(scan_radius, scan_radius, CV_8UC4);

    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, scan_radius, scan_radius);
    BITMAPINFOHEADER bi = createBitmapHeader(scan_radius);

    SelectObject(hwindowCompatibleDC, hbwindow);

    cout << "Key valid until (UTC):" << " Y" << valid_time.tm_year << " M" << valid_time.tm_mon + 1 << " D" << valid_time.tm_mday << " H" << valid_time.tm_hour << endl;
    cout << "trigger started. Hold 'XBUTTON1' to activate." << endl;

    //thread thr(revalidate_key, key, key2);

    while (true)
    {
        if (GetAsyncKeyState(VK_XBUTTON1) < 0)
        {
            get_pixels(hwindowCompatibleDC, hwindowDC, bi, hbwindow, pixel_matrix, scan_radius, x, y);
            cout << "screen captured." << endl;
            have_to_fire(pixel_matrix, scan_radius, color);
        }
    }

    return 0;
}
