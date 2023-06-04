//
// Created by YuHuanTin on 2023/6/3.
//

#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <chrono>
#include <ios>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <thread>

class WindowsCapture {
public:
    WindowsCapture()
            : dpi(getDPI()) {}
public:
    cv::Mat captureWindow(HWND Hwnd, int X = 0, int Y = 0, int W = -1, int H = -1) {
        // 获取窗口总大小
        RECT windowRect;
        GetClientRect(Hwnd, &windowRect);
        if (W == -1) W = (windowRect.right - windowRect.left) * dpi;
        if (H == -1) H = (windowRect.bottom - windowRect.top) * dpi;

        // 截图
        HDC captureDC = GetDC(Hwnd);
        HDC memDC = CreateCompatibleDC(captureDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(captureDC, W, H);
        auto oldbmp = SelectObject(memDC, hBitmap);

        if (!BitBlt(memDC, 0, 0, W, H, captureDC, X, Y, SRCCOPY))
            throw std::runtime_error("failed bitblt");

        // 拷贝数据到 cv::Mat
        cv::Mat mat(H, W, CV_8UC4);
        BITMAPINFOHEADER bi = {sizeof(bi), W, -H, 1, 32, BI_RGB};
        GetDIBits(captureDC, hBitmap, 0, H, mat.data, (BITMAPINFO *) &bi, DIB_RGB_COLORS);

        // 释放
        SelectObject(memDC, oldbmp);
        DeleteDC(memDC);
        DeleteObject(hBitmap);
        ReleaseDC(Hwnd, captureDC);
        return mat;
    }

    double getDPI() {
        HWND desktopHWND = GetDesktopWindow();
        int zoom = GetDpiForWindow(desktopHWND);
        double dpi = 0;
        switch (zoom) {
            case 96:
                dpi = 1;
                break;
            case 120:
                dpi = 1.25;
                break;
            case 144:
                dpi = 1.5;
                break;
            case 192:
                dpi = 2;
                break;
            default:
                throw std::runtime_error("bad DPI");
                break;
        }
        return dpi;
    }

private:
    double dpi;
};

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam) {
    std::string buf(256, '\0');
    GetWindowText(hwnd, buf.data(), 255);
    if (buf.find("[#] Minecraft* 1.19.4 [#]") != std::string::npos) {
        *(HWND *) (lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND findWindow() {
    HWND gameHWND = nullptr;
    EnumWindows(EnumWindowsProc, (LPARAM) &gameHWND);
    return gameHWND;
}

int main() {
    std::cout << std::unitbuf;
    HWND window = findWindow();
    WindowsCapture windowsCapture;

    cv::Mat image2 = cv::imread("..\\fp_jqsh.png");
    cv::Mat img2Gray;
    cv::cvtColor(image2, img2Gray, cv::COLOR_BGR2GRAY);
    while (true) {
        cv::Mat windowData = windowsCapture.captureWindow(window, 1750, 800, 1919 - 1750, 1029 - 800);

        cv::Mat binaryData;
        threshold(windowData, binaryData, 170, 255, cv::THRESH_BINARY);

        cv::Mat binaryData_U1;
        cv::cvtColor(binaryData, binaryData_U1, cv::COLOR_BGR2GRAY);

        // 使用模板匹配
        cv::Mat result;
        int resultWidth = binaryData_U1.cols - image2.cols + 1;
        int resultHeight = binaryData_U1.rows - image2.rows + 1;
        result.create(resultHeight, resultWidth, CV_32FC1);

        cv::matchTemplate(binaryData_U1, img2Gray, result, cv::TM_CCOEFF_NORMED);

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal < .8) {
            cv::waitKey(200);
            continue;
        }

        SetForegroundWindow(window);

        POINT curPoint;
        GetCursorPos(&curPoint);
        // 收杆 / 丢杆
        mouse_event(MOUSEEVENTF_RIGHTDOWN, curPoint.x, curPoint.y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, curPoint.x, curPoint.y, 0, 0);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        mouse_event(MOUSEEVENTF_RIGHTDOWN, curPoint.x, curPoint.y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, curPoint.x, curPoint.y, 0, 0);

        cv::waitKey(1500);
    }
    cv::destroyAllWindows();
}