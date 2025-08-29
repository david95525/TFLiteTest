// yolo_detector.cpp
#include "yolo_detector.h"
#include <android/log.h> // 如果要在 Android log

YoloDetector::YoloDetector(const std::string& model_path) {
    TfLiteModel* model = TfLiteModelCreateFromFile(model_path.c_str());
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    interpreter_ = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);

    if(interpreter_) TfLiteInterpreterAllocateTensors(interpreter_);
}

YoloDetector::~YoloDetector() {
    if(interpreter_) TfLiteInterpreterDelete(interpreter_);
}

std::vector<Detection> YoloDetector::run(const float* inputBuffer) {
    if(!interpreter_) return {};

    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(interpreter_, 0);
    TfLiteTensorCopyFromBuffer(inputTensor, inputBuffer, TfLiteTensorByteSize(inputTensor));
    if(TfLiteInterpreterInvoke(interpreter_) != kTfLiteOk) return {};

    const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(interpreter_, 0);
    const float* outputData = static_cast<const float*>(TfLiteTensorData(outputTensor));

    // 假設固定 H/W/anchors/classes
    int H=13, W=13, num_anchors=5, num_class=13;
    std::vector<std::array<float,4>> boxes;
    std::vector<std::vector<float>> class_probs;

    extract_bb(outputData, H, W, num_anchors, num_class, boxes, class_probs);
    auto detections = non_max_suppression(boxes, class_probs, 0.5f, 20);
    return detections;
}

