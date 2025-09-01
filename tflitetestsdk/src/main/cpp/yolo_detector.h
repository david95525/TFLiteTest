#pragma once
#include <vector>
#include <string>
#include "tensorflow/lite/c/c_api.h"
#include "yolo_utils.h"  // 提供 extract_bb / non_max_suppression

class YoloDetector {
public:
    // 使用已建好的 TfLiteInterpreter
    explicit YoloDetector(TfLiteInterpreter* interpreter);
    ~YoloDetector();

    // inputBuffer: 直接輸入 FloatBuffer
    std::vector<Detection> run(const float* inputBuffer);

private:
    TfLiteInterpreter* interpreter_;
};



