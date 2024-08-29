#include <atomic>
#include <opencv2/opencv.hpp>
#include <thread>

#include <gflags/gflags.h>

#include "Interpretation.h"

DEFINE_string(pre_model, "D:/wangzehui/code/github/develop/BatchLabelCrop/ImageProcessing/x64/Debug/models/mobile_sam_preprocess.onnx", "Path to the preprocessing model");
DEFINE_string(sam_model, "D:/wangzehui/code/github/develop/BatchLabelCrop/ImageProcessing/x64/Debug/models/mobile_sam.onnx", "Path to the sam model");
DEFINE_string(pre_device, "cpu", "cpu or cuda:0(1,2,3...)");
DEFINE_string(sam_device, "cpu", "cpu or cuda:0(1,2,3...)");

bool Interpretation::getInferInput(std::list<cv::Point3i> clickedPoints, std::list<cv::Point>& positvePoints, std::list<cv::Point>& negtivePoints, cv::Rect& roi)
{
    for (auto& p : clickedPoints)
    {
        if (p.z == 4 || p.z == 5)
        {
            if (roi.empty())
                roi = cv::Rect(p.x, p.y, 1, 1);
            else
            {
                auto tl = roi.tl(), np = cv::Point(p.x, p.y);
                // construct a rectangle from two points
                roi = cv::Rect(cv::Point(std::min(tl.x, np.x), std::min(tl.y, np.y)),
                    cv::Point(std::max(tl.x, np.x), std::max(tl.y, np.y)));
            }
        }
        else
        {
            if (p.z >= 2)
                positvePoints.push_back({ p.x, p.y });
            else
                negtivePoints.push_back({ p.x, p.y });
        }
    }
    if (positvePoints.empty() && negtivePoints.empty() && roi.empty()) return false;
    return true;
}

bool Interpretation::parseDeviceName(const std::string& name, Sam::Parameter::Provider& provider)
{
    if (name == "cpu") {
        provider.deviceType = 0;
        return true;
    }
    if (name.substr(0, 5) == "cuda:") {
        provider.deviceType = 1;
        provider.gpuDeviceId = std::stoi(name.substr(5));
        return true;
    }
    return false;
}

cv::Mat Interpretation::infer(cv::Mat image, int step)
{
    Sam::Parameter param(FLAGS_pre_model, FLAGS_sam_model, std::thread::hardware_concurrency());
    if (!parseDeviceName(FLAGS_pre_device, param.providers[0]) ||
        !parseDeviceName(FLAGS_sam_device, param.providers[1])) {
        std::cerr << "Unable to parse device name" << std::endl;
    }

    std::cout << "Loading model..." << std::endl;
    Sam sam(param);  // FLAGS_pre_model, FLAGS_sam_model, std::thread::hardware_concurrency());

    if (!sam.loadImage(image)) {
        std::cout << "Image loading failed" << std::endl;
    }

    cv::Size sampleSize = { image.cols / step, image.rows / step };

    std::cout << "Automatically generating masks with " << sampleSize.area()
        << " input points ..." << std::endl;

    auto mask = sam.autoSegment(
        sampleSize, [](double v) { std::cout << "\rProgress: " << int(v * 100) << "%\t"; });

    const double overlayFactor = 0.5;
    const int maxMaskValue = 255 * (1 - overlayFactor);
    cv::Mat outImage = cv::Mat::zeros(image.size(), CV_8UC3);

    static std::map<int, cv::Vec3b> colors;

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            auto value = (int)mask.at<double>(i, j);
            if (value <= 0) {
                continue;
            }
            auto it = colors.find(value);
            if (it == colors.end()) {
                colors.insert({ value, cv::Vec3b(rand() % maxMaskValue, rand() % maxMaskValue,
                                                    rand() % maxMaskValue) });
                it = colors.find(value);
            }
            outImage.at<cv::Vec3b>(i, j) = it->second + image.at<cv::Vec3b>(i, j) * overlayFactor;
        }
    }

    // draw circles on the image to indicate the sample points
    /*for (int i = 0; i < sampleSize.height; i++) {
        for (int j = 0; j < sampleSize.width; j++) {
            cv::circle(outImage, { j * step, i * step }, 2, { 0, 0, 255 }, -1);
        }
    }*/
    return outImage;
}

cv::Mat Interpretation::infer(cv::Mat image, std::list<cv::Point3i> clickedPoints)
{
    Sam::Parameter param(FLAGS_pre_model, FLAGS_sam_model, std::thread::hardware_concurrency());
    if (!parseDeviceName(FLAGS_pre_device, param.providers[0]) ||
        !parseDeviceName(FLAGS_sam_device, param.providers[1])) {
        std::cerr << "Unable to parse device name" << std::endl;
    }

    std::cout << "Loading model..." << std::endl;
    Sam sam(param);  // FLAGS_pre_model, FLAGS_sam_model, std::thread::hardware_concurrency());

    if (!sam.loadImage(image)) {
        std::cout << "Image loading failed" << std::endl;
    }


    std::list<cv::Point> positivePoints, negativePoints;
	cv::Rect roi;
    if (!getInferInput(clickedPoints, positivePoints, negativePoints, roi))
        std::cout << "Parse prompt point failed" << std::endl;
    cv::Mat mask = sam.getMask(positivePoints, negativePoints, roi);

    // apply mask to image
    cv::Mat outImage = cv::Mat::zeros(image.size(), CV_8UC3);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            auto bFront = mask.at<uchar>(i, j) > 0;
            float factor = bFront ? 1.0 : 0.2;
            outImage.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j) * factor;
        }
    }

    //绘制正样本点
    for (auto& p : positivePoints) {
        cv::circle(outImage, p, 2, { 0, 255, 255 }, -1);
    }
    //绘制负样本点
    for (auto& p : negativePoints) {
        cv::circle(outImage, p, 2, { 255, 0, 0 }, -1);
    }
    //绘制白色的框
    if (!roi.empty()) {
        cv::rectangle(outImage, roi, { 255, 255, 255 }, 2);
    }
    return outImage;
}