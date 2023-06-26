#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ios>
#include <iostream>
#include <Windows.h>
#include <set>
#include <string>

struct warpCvVec4b : public cv::Vec4b {
    using cv::Vec4b::Vec;

    bool operator<(const warpCvVec4b &R) const {
        if (this->val[0] != R.val[0]) return this->val[0] < R.val[0];
        if (this->val[1] != R.val[1]) return this->val[1] < R.val[1];
        if (this->val[2] != R.val[2]) return this->val[2] < R.val[2];
        return false;
    }
};


// 获取系统DPI缩放值
extern double getDPI();

// 创建cv窗口
extern HWND createShowWindow(double);

// 寻找窗口
extern HWND findWindow();

// 对窗口区域进行截图
extern cv::Mat getWindowCaptureData(HWND, HWND, double);

// 处理遮罩
extern cv::Mat dataFilter(cv::Mat &, double, std::set<warpCvVec4b> &);

extern std::set<warpCvVec4b> readConfig();

extern void writeConfig();

const std::string windowTitle = "TerrariaCVColorBlock";

int main() {
    std::cout << std::unitbuf;
    try {
        double dpi = getDPI();

        writeConfig();
        std::set<warpCvVec4b> findColors = readConfig();

        HWND cvWindowHWND = createShowWindow(dpi);
        HWND terrariaWindowHWND = findWindow();

        while (true) {
            if (GetForegroundWindow() == terrariaWindowHWND) {
                SetLayeredWindowAttributes(cvWindowHWND, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
                cv::Mat data = getWindowCaptureData(cvWindowHWND, terrariaWindowHWND, dpi);
                data = dataFilter(data, dpi, findColors);
                cv::imshow(windowTitle, data);
                cv::waitKey(20);
            } else {
                SetLayeredWindowAttributes(cvWindowHWND, RGB(0, 0, 0), 0, LWA_ALPHA | LWA_COLORKEY);
                cv::waitKey(500);
            }
        }
    } catch (std::runtime_error &error) {
        std::cout << "error: " << error.what() << '\n';
    }
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void writeConfig() {
    struct findPixel {
        std::string name; // require unique
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t alpha;
    };
    std::vector<findPixel> insertJsonDatas{
//            {u8"精金矿",   128,  26,  52, 0xff},
//            {u8"宝箱", 233, 207, 94, 0xff},
//            {u8"世纪之花", 225, 128, 206, 0xff},
            {u8"生命果", 149, 232, 87, 0xff}
    };
    nlohmann::json nJson;
    for (int i = 0; i < insertJsonDatas.size(); ++i) {
        nJson[i][insertJsonDatas[i].name]["r"] = insertJsonDatas[i].r;
        nJson[i][insertJsonDatas[i].name]["g"] = insertJsonDatas[i].g;
        nJson[i][insertJsonDatas[i].name]["b"] = insertJsonDatas[i].b;
        nJson[i][insertJsonDatas[i].name]["alpha"] = insertJsonDatas[i].alpha;
    }

    std::fstream fstream("config.json", std::ios_base::out | std::ios_base::binary);
    std::string tmp = nJson.dump(4);
    fstream.write(tmp.data(), tmp.length());
    fstream.close();
}

std::set<warpCvVec4b> readConfig() {
    std::set<warpCvVec4b> jsonElements;

    std::fstream fstream("config.json");
    if (!fstream) throw std::runtime_error("can't find config.json at current directory");
    fstream.seekg(0, std::ios_base::end);
    size_t fileSize = fstream.tellg();
    fstream.seekg(0, std::ios_base::beg);
    std::string tmp(fileSize, '\0');
    fstream.read(tmp.data(), fileSize);

    nlohmann::json nJson(nlohmann::json::parse(tmp));

    for (const auto &item: nJson) {
        for (const auto &element: item.items()) {
            std::cout << "Name: " << element.key() << std::endl;
            const auto &color = element.value();

            warpCvVec4b p2(color["b"], color["g"], color["r"], color["alpha"]);
            if (!jsonElements.insert(p2).second) throw std::runtime_error("failed insert to set");
        }
    }
    if (jsonElements.empty()) throw std::runtime_error("json element is empty");
    return jsonElements;
}

cv::Mat dataFilter(cv::Mat &MatData, double DPI, std::set<warpCvVec4b> &RequestColors) {
    auto &h = MatData.rows;
    auto &w = MatData.cols;

    std::vector<cv::Point> targetPixels;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto &color = MatData.at<cv::Vec4b>(y, x);
            auto &tr = (warpCvVec4b &) color;
            if (RequestColors.count(tr) == 1) {
                targetPixels.emplace_back(x, y);
                continue;
            }
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
            color[3] = 0;
        }
    }
    for (auto &targetPixel: targetPixels) {
        POINT curPoint;
        GetCursorPos(&curPoint);
        cv::line(MatData, targetPixel, cv::Point(curPoint.x * DPI, curPoint.y * DPI - 22),
                 cv::Scalar(0, 0, 255), 1);
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

HWND findWindow() {
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