//
// Created by YuHuanTin on 2023/6/3.
//

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/opencv.hpp>


int main() {

    auto p = cv::imread("D:\\我的文件\\IDM\\下载文件-IDM\\2023-6-3 21-40-7.png");

    cv::namedWindow("img", cv::WINDOW_NORMAL);
    cv::imshow("img", p);

    cv::waitKey(0);
    cv::destroyAllWindows();
}