void openCPUTurbo() {
  
    // 使用 ThrottleStop 服务
    // 注意: 使用该服务前请先启动服务!
    HANDLE hDevice = CreateFile(R"(\\.\ThrottleStop)", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, nullptr);

    // open turbo
    DWORD ret = 0;
    DWORD input[3] = {0x1A0, 0x850089, 0};
    DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);

    // disabled C1E
    input[0] = 0x1FC;
    input[1] = 0xEC0059;
    DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);

    // enabled C1E
    input[1] = 0xEC005B;
    DeviceIoControl(hDevice, 0x8000644C, &input, 0xC, 0, 0, &ret, nullptr);

}

int main(int argc, const char *argv[]) {

    openCPUTurbo();
}
