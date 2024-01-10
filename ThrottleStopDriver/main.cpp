#include <windows.h>
#include <iostream>

void cpuSetting(bool DisableC1E = true) {

    // 注意: 请先手动注册 ThrottleStop 驱动,这里推荐使用 ProcessHack (github有) '创建服务',类型为'驱动',启动类型为'需求启动'
    system("sc start ThrottleStop");

    HANDLE hDevice = CreateFile(R"(\\.\ThrottleStop)", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, nullptr);

    /**
    DWORD a = 0x1A0;
    DWORD b[2];
    DWORD ret = 0;
    DeviceIoControl(hDevice, 0x80006448, &a, 4, &b, 8, &ret, nullptr);
    if (ret == 0) {
        std::cout << "failed get param!\n";
        return;
    }
     */

    // open turbo
    DWORD ret = 0;
    DWORD input[3] = {0x1A0, 0x850089, 0};
    DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);
    std::cout << "open turbo code: " << ret << '\n';
    if (DisableC1E) {
        // disabled C1E
        input[0] = 0x1FC;
        input[1] = 0xEC0059;
        DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);

        std::cout << "disabled C1E code: " << ret << '\n';
    } else {
        // enabled C1E
        input[0] = 0x1FC;
        input[1] = 0xEC005B;
        DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);

        std::cout << "enabled C1E code: " << ret << '\n';
    }

    system("sc stop ThrottleStop");
}

int main(int argc, const char *argv[]) {

    cpuSetting(true);
    
    return 0;
}
