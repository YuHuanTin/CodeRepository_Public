#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <vector>

class WindowsCapture {
public:
    WindowsCapture() : dpi(getDPI()) {}

public:
    // Capture a picture from the specified hwnd window
    cv::Mat captureWindow(HWND Hwnd, int X = 0, int Y = 0, int W = -1, int H = -1) const {
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

int main() {
    std::cout << std::unitbuf;

    // load images res
    std::vector<cv::Mat> res;
    std::filesystem::directory_iterator begin(std::filesystem::current_path().string() + "\\res");
    std::filesystem::directory_iterator end;
    while (begin != end) {
        res.emplace_back(cv::imread(begin->path().string()));
        ++begin;
    }
    std::cout << "read file count: " << res.size() << '\n';

    // match image
    while (true) {
        cv::Mat wantSearch = cv::imread("search.jpg");

        cv::Mat lastMaxMat;
        double lastMaxVal = -1;
        for (auto &one: res) {

            // create a resulting graph, but possibly with errors
            cv::Mat result;
            cv::matchTemplate(one, wantSearch, result, cv::TM_CCOEFF_NORMED);

            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal > lastMaxVal) {
                lastMaxMat = one;
                lastMaxVal = maxVal;
                std::cout << "p maxVal: " << maxVal << '\n';
            }
        }

        cv::imshow("img", wantSearch);
        cv::imshow("best", lastMaxMat);

        cv::waitKey(0);
        cv::destroyAllWindows();
    }
}