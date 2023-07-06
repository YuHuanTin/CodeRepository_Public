
#include "fmt/core.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <exception>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <WinhttpAPI.h>


const std::string titleName = "rawImage";
const std::string imageDownloadPath = "F:\\tmp.jpg";
const std::string saveDatasetDir = "F:\\dataset";
const std::string saveDatasetWithoutLabelDir = "F:\\dataset_not_label";
// 预测的图片目录
const std::string predictDir = "F:\\dataset_not_label";

void initFileFolder() {
    std::filesystem::create_directories(saveDatasetDir);
    std::filesystem::create_directories(saveDatasetWithoutLabelDir);

}

bool writeImage() {
    using namespace Winhttp;
    try {
        HttpRequestT requestT{"http://ptlogin.4399.com/ptlogin/captcha.do?captchaId=captchaReq324429cee4c96783724",
                              "get",
                              {HttpRequestT::SaveMethodT::FILE_STREAM, imageDownloadPath}};
        HttpResponseT responseT;
        WinhttpAPI api(requestT, responseT);
        api.Request();
    } catch (std::exception &exception) {
        fmt::println("error: {}", exception.what());
        return false;
    }
    return true;
}

bool saveImage(const std::string &ImageText) {
    try {
        auto currentTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

        std::filesystem::path tmpFile(imageDownloadPath);
        std::filesystem::path newFilePath(saveDatasetDir);
        newFilePath /= ImageText + '_' + std::to_string(currentTimeStamp) + ".jpg";

        rename(tmpFile, newFilePath);
    } catch (std::exception &exception) {
        fmt::println("error: {}", exception.what());
        return false;
    }
    return true;
}

bool saveImage() {
    try {
        auto currentTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

        std::filesystem::path tmpFile(imageDownloadPath);
        std::filesystem::path newFilePath(saveDatasetWithoutLabelDir);
        newFilePath /= std::to_string(currentTimeStamp) + ".jpg";
        rename(tmpFile, newFilePath);
    } catch (std::exception &exception) {
        fmt::println("error: {}", exception.what());
        return false;
    }
    return true;
}

std::optional<std::string> showImageAndGetInput(const cv::Mat &Image) {
    using namespace cv;
    try {
        imshow(titleName, Image);
        waitKey(10);

        std::string inputText;
        std::thread inputThread([&]() { std::cin >> inputText; });
        inputThread.join();
        destroyAllWindows();

        return inputText;
    } catch (std::exception &exception) {
        fmt::println("error: {}", exception.what());
        return std::nullopt;
    }
}

namespace captchaOperator {

    /**
     *  手动标记图片
     */
    void labelingImages() {
        using namespace cv;
        using namespace std;
        try {
            namedWindow(titleName, WINDOW_KEEPRATIO | WINDOW_NORMAL);
            HWND hWindow = FindWindowA(nullptr, titleName.c_str());

            /**
             *  设置窗口大小
             */
            RECT rect;
            GetWindowRect(hWindow, &rect); // 获取窗口的矩形区域
            SetWindowPos(hWindow, nullptr, rect.left, rect.top, 120, 60, 0);

            Mat currentImg = imread(imageDownloadPath, IMREAD_UNCHANGED);
            auto ret = showImageAndGetInput(currentImg);
            if (!ret) throw std::runtime_error("bad input because non value");
            std::string input = ret.value();
            if (input.length() == 4) {
                saveImage(input);
                fmt::println("input value: {}", input);
            }
        } catch (std::exception &exception) {
            fmt::println("error: {}", exception.what());
        }
    }

    /**
     *  判断图片是否为正确，如果是正确图片放到 success_label 目录
     *  如果图片不正确放到 failed_label 目录
     */
    void filterImages() {
        using namespace cv;
        try {
            std::filesystem::path predict(predictDir);
            std::filesystem::create_directories(predict / "success_label");
            std::filesystem::create_directories(predict / "failed_label");

            for (auto &file: std::filesystem::directory_iterator(predict)) {
                if (file.is_directory()) continue;

                std::string fileName = file.path().filename().string();
//                fmt::println("filename: {}", fileName);
                fmt::println("predict label: {}", fileName.substr(0, 4));


                namedWindow(titleName, WINDOW_KEEPRATIO | WINDOW_NORMAL);
                HWND hWindow = FindWindowA(nullptr, titleName.c_str());

                /**
                 *  设置窗口大小
                 */
                RECT rect;
                GetWindowRect(hWindow, &rect); // 获取窗口的矩形区域
                SetWindowPos(hWindow, nullptr, 880, rect.top, 0, 0, 0);

                Mat image = imread(file.path().string(), IMREAD_UNCHANGED);
                auto ret = showImageAndGetInput(image);
                if (!ret) throw std::runtime_error("bad input because non value");

                if (ret.value() == "0") {
                    remove(file.path().string().c_str());
                } else if (ret.value().length() != 4) {
                    rename(file.path().string(), (predict / "success_label") / fileName);
                } else if (ret->length() == 4){
                    fileName.insert(0, ret.value());
                    fileName.erase(4, 4);
                    fmt::println("new file name: {}", fileName);
                    rename(file.path().string(), (predict / "failed_label") / fileName);
                }

            }
        } catch (std::exception &exception) {
            fmt::println("error: {}", exception.what());
        }


    }
}


int main() {
    using namespace cv;
    using namespace std;
    setbuf(stdout, nullptr);

    initFileFolder();
    captchaOperator::filterImages();


}