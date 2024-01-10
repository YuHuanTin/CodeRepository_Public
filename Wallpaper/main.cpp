#include <iostream>
#include <cstdio>
#include <string>
#include "Windows.h"

double gIconAttributes = 100;

BOOL cb(HWND hwnd, LPARAM lparam) {
    // �����洰���µĴ�����Ϊ������(��Progmanͬ��)Ѱ��SHELLDLL_DefViewΪ�����Ĵ���
    HWND hDefView = FindWindowEx(hwnd, nullptr, "SHELLDLL_DefView", nullptr);

    // ���hDefView��Ϊnullptr��˵����WorkerW��������SHELLDLL_DefView����
    if (hDefView != nullptr) {
        // ��������һ������,Ϊû��SHELLDLL_DefView��WorkerW����
        HWND hWorkerw = FindWindowEx(nullptr, hwnd, "WorkerW", nullptr);

        // ��������ͼ��͸����
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        SetLayeredWindowAttributes(hwnd, 0, (int)(gIconAttributes / 100 * 255), LWA_ALPHA);
        //UpdateLayeredWindow(hwnd, nullptr, nullptr, nullptr, nullptr, nullptr, 0, nullptr, 0);

        HWND *p = (HWND *) lparam;
        *p = hWorkerw;
        return false;
    }
    return true;
}

void setWallpaper(const std::string &playerClassName) {
    // �ҵ�explorer�����洰��
    HWND hProgman = FindWindow("Progman", nullptr);

    // �����洰�ڷ�����Ϣ
    // ���Progman���ѳ�����WorkerW�����Ĵ���,һ���ŵ��Ǳ�ֽһ���ŵ���ͼ���
    SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 10000, nullptr);

    // �ҵ����ѳ����ĵڶ���WorkerW�Ĵ��ھ��
    HWND hSecondWorker = nullptr;
    EnumWindows(cb, (LPARAM) &hSecondWorker);
    // �ҵ��Լ�������Ƶ�Ĵ���
    HWND hFfplay = FindWindow(playerClassName.c_str(), nullptr);

    // ���ò�����Ƶ�Ĵ�������ΪProgman���Ӵ���
    SetParent(hFfplay, hSecondWorker);


    // ��ֹת�����ڵ�����ʧ��
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

    std::cout << "�Ƿ���Ҫ����ͼ��͸����(�����Ҫ����������0-100��ֵ):";
    std::cin >> s;
    long value = strtol(s.c_str(), nullptr, 10);
    if (0 <= value && value <= 100) {
        gIconAttributes = value;
        std::cout << "͸����������Ϊ: " << gIconAttributes << '\n';
    }

    std::cout << "�������ֽ��·��:";
    std::cin >> s;
    std::string lpParameter =
            " " + s + " -noborder -x " + std::to_string(width) + " -y " + std::to_string(height) + " -loop 0";

    std::cout << "������ffplay·��:";
    std::cin >> s;
    if (s.front() == '"' && s.back() == '"') {
        s = s.substr(1, s.length() - 2);
    }

    // ����ffplay
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {nullptr};
    if (CreateProcess(s.c_str(), lpParameter.data(), nullptr, nullptr, 0, 0, nullptr, nullptr, &si, &pi)) {
        Sleep(1000);
        setWallpaper("SDL_app");
    }

    Sleep(2000);

    return 0;
}
