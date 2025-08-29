// yolo_detector.h
#pragma once
#include <string>
#include <vector>
#include <array>
#include "yolo_utils.h"
#include "tensorflow/lite/c/c_api.h"

class YoloDetector {
public:
    YoloDetector(const std::string& model_path);
    ~YoloDetector();

    // 將圖片資料傳入（已轉 float buffer）
    std::vector<Detection> run(const float* inputBuffer);

private:
    TfLiteInterpreter* interpreter_;
};

