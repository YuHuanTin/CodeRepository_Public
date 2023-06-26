//
// Created by YuHuanTin on 2023/6/10.
//

#include "opencv2/highgui.hpp"
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <exception>
#include <iostream>
#include <random>
#include <unordered_map>

struct Randomer {
    std::size_t blockHeight = 0;
    std::size_t blockWidth = 0;
    std::size_t randomSeed = 0;
};

void showImageInfo(cv::Mat &Image) {
    fmt::println("image height: {}\nimage width: {}", Image.rows, Image.cols);
    std::vector<std::size_t> splitSize;
    for (int i = 1; i <= Image.rows; ++i) {
        if (Image.rows % i == 0) {
            splitSize.emplace_back(i);
        }
    }
    fmt::println("block height can set: {}", splitSize);
    splitSize.clear();
    for (int i = 1; i <= Image.cols; ++i) {
        if (Image.cols % i == 0) {
            splitSize.emplace_back(i);
        }
    }
    fmt::println("block width can set: {}", splitSize);
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

/**
 *  // 展示图片信息 输出可选的分块大小\n
 *  ImageShuffle.exe -type info -i input.png
 *  \n\n
 *  // 对图像进行打乱\n
 *  ImageShuffle.exe -i input.png -o output.png -w 1 -h 1 -seed 123456 -type encode
 *  \n\n
 *  // 对图片进行还原\n
 *  ImageShuffle.exe -i input.png -o output.png -w 1 -h 1 -seed 123456 -type decode
 *  \n\n
 *  -type           indicate what to do\n
 *  -i              inputImage\n
 *  -o              outputImage         optional (output.png)\n
 *  -w              blockWidth          optional (1)\n
 *  -h              blockHeight         optional (1)\n
 *  -seed           randomSeed          optional (31415926)\n
 *  \n
 *  Warning, please do not convert to jpg, the color may change, it is recommended to convert to png format\n
 */

int main(int argc, char *argv[]) {
    using namespace cv;
    std::cout << std::unitbuf;

    if (argc == 1) {
        fmt::println("展示图片信息 输出可选的分块大小\n"
                     "ImageShuffle.exe -type info -i input.png\n"
                     "对图像进行打乱\n"
                     "ImageShuffle.exe -i input.png -o output.png -w 1 -h 1 -seed 123456 -type encode\n"
                     "对图片进行还原\n"
                     "ImageShuffle.exe -i input.png -o output.png -w 1 -h 1 -seed 123456 -type decode\n\n"
                     "-type           indicate what to do\n"
                     "-i              inputImage\n"
                     "-o              outputImage         optional (output.png)\n"
                     "-w              blockWidth          optional (1)\n"
                     "-h              blockHeight         optional (1)\n"
                     "-seed           randomSeed          optional (31415926)\n"
                     "Warning, please do not convert to jpg, the color may change, it is recommended to convert to png format\n"
        );
        std::getchar();
        exit(0);
    }
    try {
        // load args
        std::unordered_map<std::string, std::string> args;
        for (int i = 0; i < argc; ++i) {
            if (argv == nullptr) throw std::runtime_error("bad argument because of null pointer");
            if (argv[i][0] == '-') {
                if (argv[i + 1] == nullptr || argv[i + 1][0] == '-')
                    throw std::runtime_error("arguments with no options provided");
                else args[argv[i] + 1] = argv[i + 1];
            }
        }

        fmt::println("args: {}", args);

        // parse args
        std::string &type = args.at("type");

        // operate on image
        Mat inputImage, outputImage;
        Randomer randomer;
        inputImage = imread(args.at("i"));
        if (type == "info") {
            showImageInfo(inputImage);
            exit(0);
        } else if (type == "encode") {
            randomer.randomSeed = args.find("seed") != args.end()
                                  ? std::strtoll(args.at("seed").c_str(), nullptr, 10)
                                  : 31415926;

            randomer.blockWidth = args.find("w") != args.end()
                                  ? std::strtoll(args.at("w").c_str(), nullptr, 10)
                                  : 1;

            randomer.blockHeight = args.find("h") != args.end()
                                   ? std::strtoll(args.at("h").c_str(), nullptr, 10)
                                   : 1;
            outputImage = encode(inputImage, randomer);
        } else if (type == "decode") {
            randomer.randomSeed = args.find("seed") != args.end()
                                  ? std::strtoll(args.at("seed").c_str(), nullptr, 10)
                                  : 31415926;

            randomer.blockWidth = args.find("w") != args.end()
                                  ? std::strtoll(args.at("w").c_str(), nullptr, 10)
                                  : 1;

            randomer.blockHeight = args.find("h") != args.end()
                                   ? std::strtoll(args.at("h").c_str(), nullptr, 10)
                                   : 1;
            outputImage = decode(inputImage, randomer);
        } else {
            throw std::runtime_error("bad type provided");
        }

        // specify the default output filename
        if (args.find("o") == args.end()) args["o"] = "output.png";
        imwrite(args.at("o"), outputImage);
    } catch (std::out_of_range &out_of_range) {
        fmt::println("key value not found, please check help\n{}", out_of_range.what());
    } catch (std::exception &exception) {
        fmt::println("{}", exception.what());
    }
}