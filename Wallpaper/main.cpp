#include <iostream>
#include <cstdio>
#include <string>
#include "Windows.h"

BOOL cb(HWND hwnd, LPARAM lparam) {
    // 以桌面窗口下的窗口作为父窗口(与Progman同级)寻找SHELLDLL_DefView为类名的窗口
    HWND hDefView = FindWindowEx(hwnd, nullptr, "SHELLDLL_DefView", nullptr);

    // 如果hDefView不为nullptr则说明该WorkerW窗口下有SHELLDLL_DefView窗口
    if (hDefView != nullptr) {
        // 找它的下一个窗口,为没有SHELLDLL_DefView的WorkerW窗口
        HWND hWorkerw = FindWindowEx(nullptr, hwnd, "WorkerW", nullptr);

        HWND *p = (HWND *) lparam;
        *p = hWorkerw;
        return false;
    }
    return true;
}

void setWallpaper(const std::string &playerClassName) {
    // 找到explorer的桌面窗口
    HWND hProgman = FindWindow("Progman", nullptr);

    // 向桌面窗口发送消息
    // 会把Progman分裂出两个WorkerW类名的窗口,一个放的是壁纸一个放的是图标等
    SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 10000, nullptr);

    // 找到分裂出来的第二个WorkerW的窗口句柄
    HWND hSecondWorker = nullptr;
    EnumWindows(cb, (LPARAM) &hSecondWorker);
    // 找到自己播放视频的窗口
    HWND hFfplay = FindWindow(playerClassName.c_str(), nullptr);

    // 设置播放视频的窗口设置为Progman的子窗口
    SetParent(hFfplay, hSecondWorker);


    // 防止转到窗口但是消失了
    SetWindowLongPtr(hFfplay, GWL_EXSTYLE, WS_EX_NOACTIVATE);
    SetWindowLongPtr(hFfplay, GWL_STYLE, WS_CHILD | WS_VISIBLE);
}

int main(int argc, char *argv[]) {
    setbuf(stdout, nullptr);
    HWND hwd = GetDesktopWindow();
    HDC hdc = GetDC(hwd);
    int width = GetDeviceCaps(hdc, DESKTOPHORZRES);
    int height = GetDeviceCaps(hdc, DESKTOPVERTRES);
    std::cout << "screen x: " << width << "\nscreen y: " << height << '\n';

    std::string s;
    std::cout << "请输入壁纸的路径:";
    std::cin >> s;
    std::string lpParameter =
            " " + s + " -noborder -x " + std::to_string(width) + " -y " + std::to_string(height) + " -loop 0";

    std::cout << "请输入ffplay路径:";
    std::cin >> s;

    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {nullptr};
    if (CreateProcess(s.c_str(), lpParameter.data(), nullptr, nullptr, 0, 0, nullptr, nullptr, &si, &pi)) {
        Sleep(1000);
        setWallpaper("SDL_app");

    }
    getchar();

    return 0;
}
