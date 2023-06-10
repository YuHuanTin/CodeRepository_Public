//
// Created by YuHuanTin on 2023/6/10.
//
#include "fmt/core.h"
#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include <algorithm>
#include <cstddef>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <random>
#include <vector>

struct Randomer {
    std::size_t blockHeight = 0;
    std::size_t blockWidth = 0;
    std::size_t randomSeed = 0;
};

void showImageInfo(cv::Mat &Image) {
    fmt::println("image height: {}\nimage width: {}", Image.rows, Image.cols);
    for (int i = 2; i <= Image.rows; ++i) {
        if (Image.rows % i == 0) {
            fmt::println("height can split with: {}", i);
        }
    }
    for (int i = 2; i <= Image.cols; ++i) {
        if (Image.cols % i == 0) {
            fmt::println("width can split with: {}", i);
        }
    }
}

cv::Mat encode(cv::Mat &Image, const Randomer &RandomerParam) {
    cv::Mat result(Image.size(), Image.type());
    std::size_t heightCount = Image.rows / RandomerParam.blockHeight;
    std::size_t widthCount = Image.cols / RandomerParam.blockWidth;

    std::vector<std::size_t> ord(heightCount * widthCount);
    for (int i = 0; i < heightCount * widthCount; ++i) {
        ord[i] = i;
    }

    std::default_random_engine randomEngine(RandomerParam.randomSeed);
    std::shuffle(ord.begin(), ord.end(), randomEngine);
    for (int i = 0; i < heightCount; ++i) {
        for (int j = 0; j < widthCount; ++j) {
            cv::Mat srcROI = Image(cv::Rect(j * RandomerParam.blockWidth,
                                            i * RandomerParam.blockHeight,
                                            RandomerParam.blockWidth, RandomerParam.blockHeight));

            auto toIndex = ord[i * widthCount + j];
            srcROI.copyTo(result(cv::Rect((toIndex % widthCount) * RandomerParam.blockWidth,
                                          toIndex / widthCount * RandomerParam.blockHeight,
                                          RandomerParam.blockWidth, RandomerParam.blockHeight)));
        }
    }
    return result;
}

cv::Mat decode(cv::Mat &Image, const Randomer &RandomerParam) {
    cv::Mat result(Image.size(), Image.type());
    std::size_t heightCount = Image.rows / RandomerParam.blockHeight;
    std::size_t widthCount = Image.cols / RandomerParam.blockWidth;

    std::vector<std::size_t> ord(heightCount * widthCount);
    for (int i = 0; i < heightCount * widthCount; ++i) {
        ord[i] = i;
    }

    std::default_random_engine randomEngine(RandomerParam.randomSeed);
    std::shuffle(ord.begin(), ord.end(), randomEngine);

    for (int i = 0; i < heightCount; ++i) {
        for (int j = 0; j < widthCount; ++j) {
            auto toIndex = ord[i * widthCount + j];
            cv::Mat srcROI = Image(cv::Rect((toIndex % widthCount) * RandomerParam.blockWidth,
                                            toIndex / widthCount * RandomerParam.blockHeight,
                                            RandomerParam.blockWidth, RandomerParam.blockHeight));

            srcROI.copyTo(result(cv::Rect(j * RandomerParam.blockWidth,
                                          i * RandomerParam.blockHeight,
                                          RandomerParam.blockWidth, RandomerParam.blockHeight)));
        }
    }
    return result;
}

int main() {
    using namespace cv;
    Mat img = imread("D:\\我的文件\\IDM\\下载文件-IDM\\0.png");

//    showImageInfo(img);

    Randomer randomerParam;
    randomerParam.blockHeight = 1;
    randomerParam.blockWidth = 1;
    randomerParam.randomSeed = 11111;
    Mat result = encode(img, randomerParam);

    Mat raw = decode(result, randomerParam);

    namedWindow("en", WINDOW_NORMAL);
    namedWindow("de", WINDOW_NORMAL);
    imshow("en", result);
    imshow("de", raw);
    waitKey(0);
}