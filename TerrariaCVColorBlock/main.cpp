#include <chrono>
#include <ios>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string>
#include <vector>

// 获取系统DPI缩放值
extern double getDPI();

// 创建cv窗口
extern HWND createShowWindow(double);

extern HWND findTerrariaWindow();

// 对窗口区域进行截图
extern cv::Mat getWindowCaptureData(HWND, HWND, double);

// 处理遮罩
extern cv::Mat dataFilter(cv::Mat &, double);

const std::string windowTitle = "TerrariaCVColorBlock";

int main() {
    std::cout << std::unitbuf;
    try {
        double dpi = getDPI();
        HWND cvWindowHWND = createShowWindow(dpi);
        HWND terrariaWindowHWND = findTerrariaWindow();
        while (true) {
            if (GetForegroundWindow() == terrariaWindowHWND) {
                SetLayeredWindowAttributes(cvWindowHWND, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
                cv::Mat data = getWindowCaptureData(cvWindowHWND, terrariaWindowHWND, dpi);
                data = dataFilter(data, dpi);
                cv::imshow(windowTitle, data);
                cv::waitKey(20);
            } else {
                SetLayeredWindowAttributes(cvWindowHWND, RGB(0, 0, 0), 0, LWA_ALPHA | LWA_COLORKEY);
                cv::waitKey(5000);
            }
        }
    } catch (std::runtime_error &error) {
        std::cout << "error: " << error.what() << '\n';
    }
    cv::waitKey(0);
    cv::destroyAllWindows();
}

cv::Mat dataFilter(cv::Mat &MatData, double DPI) {
    auto &h = MatData.rows;
    auto &w = MatData.cols;

    std::vector<cv::Point> targetPixels;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto &color = MatData.at<cv::Vec4b>(y, x);
            cv::Vec4b jingjin(52, 26, 128, 0xff);
            cv::Vec4b glodenBoxLocked(94, 207, 233, 0xff);
            if (glodenBoxLocked == color || jingjin == color) {
                targetPixels.emplace_back(x, y);
                continue;
            }
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
            color[3] = 0;
        }
    }
    for (auto &targetPixel : targetPixels) {
        POINT curPoint;
        GetCursorPos(&curPoint);
        cv::line(MatData, targetPixel, cv::Point(curPoint.x * DPI, curPoint.y * DPI - 22),
                 cv::Scalar(0, 0, 255), 2);
    }
    return MatData;
}

cv::Mat getWindowCaptureData(HWND CvWindowHWND, HWND CaptureWindowHWND, double DPI) {
    RECT cvWindowRect;
    GetClientRect(CvWindowHWND, &cvWindowRect);
    int cvWindowHeight = (cvWindowRect.bottom - cvWindowRect.top) * DPI;
    int cvWindowWidth = (cvWindowRect.right - cvWindowRect.left) * DPI;

    RECT captureWindowRect;
    GetClientRect(CaptureWindowHWND, &captureWindowRect);
    int captureWindowHeight = (captureWindowRect.bottom - captureWindowRect.top) * DPI;
    int captureWindowWidth = (captureWindowRect.right - captureWindowRect.left) * DPI;

    // 截图
    HDC captureDC = GetDC(CaptureWindowHWND);
    HDC memDC = CreateCompatibleDC(captureDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(captureDC, cvWindowWidth, cvWindowHeight);
    auto oldbmp = SelectObject(memDC, hBitmap);

    BitBlt(memDC, 0, 0, captureWindowWidth, captureWindowHeight, captureDC, 0, 0, SRCCOPY);

    // 拷贝数据到cv::Mat
    cv::Mat mat(cvWindowHeight, cvWindowWidth, CV_8UC4);
    BITMAPINFOHEADER bi = {sizeof(bi), cvWindowWidth, -cvWindowHeight, 1, 32, BI_RGB};
    GetDIBits(captureDC, hBitmap, 0, cvWindowHeight, mat.data, (BITMAPINFO *) &bi, DIB_RGB_COLORS);

    // 释放
    SelectObject(memDC, oldbmp);
    DeleteDC(memDC);
    DeleteObject(hBitmap);
    ReleaseDC(CaptureWindowHWND, captureDC);
    return mat;
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam) {
    std::string buf(256, '\0');
    GetWindowText(hwnd, buf.data(), 255);
    if (buf.find("泰拉瑞亚") != std::string::npos) {
        *(HWND *) (lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND findTerrariaWindow() {
    HWND gameHWND = nullptr;
    EnumWindows(EnumWindowsProc, (LPARAM) &gameHWND);
    return gameHWND;
}

HWND createShowWindow(double DPI) {
    cv::namedWindow(windowTitle, cv::WINDOW_NORMAL);

    HWND cvWindowHWND = FindWindow(nullptr, windowTitle.c_str());
    if (cvWindowHWND == nullptr) throw std::runtime_error("can't find window");


    int iWidth = GetSystemMetrics(SM_CXSCREEN) * DPI;
    int iHeight = GetSystemMetrics(SM_CYSCREEN) * DPI;

    // 去掉标题栏及边框
    long Style = GetWindowLongPtr(cvWindowHWND, GWL_STYLE);
    Style = Style & ~WS_CAPTION & ~WS_SYSMENU & ~WS_SIZEBOX;
    SetWindowLongPtr(cvWindowHWND, GWL_STYLE, Style);

    // 至顶层窗口 最大化
    SetWindowPos(cvWindowHWND, HWND_TOPMOST, 0, 22, iWidth, iHeight, SWP_SHOWWINDOW);

    // 设置窗口透明
    SetWindowLong(cvWindowHWND, GWL_EXSTYLE, WS_EX_LAYERED);
    SetLayeredWindowAttributes(cvWindowHWND, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);

    // 设置鼠标穿透
    SetWindowLong(cvWindowHWND, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
    return cvWindowHWND;
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
