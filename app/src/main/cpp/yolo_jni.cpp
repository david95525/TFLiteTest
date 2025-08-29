#include <string>       // std::string
#include <vector>       // std::vector
#include <array>        // std::array
#include "yolo_detector.h"
#include "yolo_utils.h"
extern "C" {
YoloDetector* yolo_create(const char* model_path) {
    return new YoloDetector(model_path);
}

void yolo_destroy(YoloDetector* detector) {
    delete detector;
}

const char* yolo_run(YoloDetector* detector, const float* inputBuffer) {
    static std::string result;
    auto detections = detector->run(inputBuffer);
    result.clear();
    for(auto& d : detections) {
        result += "class=" + std::to_string(d.class_id) +
                  " prob=" + std::to_string(d.score) + "\n";
    }
    return result.c_str();
}
}
