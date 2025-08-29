#pragma once
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

// YOLO Detection 結果結構
struct Detection {
    std::array<float,4> box;  // [x, y, w, h]
    int class_id;
    float score;
};

// Logistic / Sigmoid
float logistic(float x);

// Extract bounding boxes
void extract_bb(const float* prediction_output, int H, int W, int num_anchors, int num_class,
                std::vector<std::array<float,4>>& boxes,
                std::vector<std::vector<float>>& class_probs);

// Non-maximum suppression
std::vector<Detection> non_max_suppression(const std::vector<std::array<float,4>>& boxes,
                                           const std::vector<std::vector<float>>& class_probs,
                                           float prob_threshold, int max_detections=10);

// Softmax
void softmax(float* data, int length);

// IoU
float iou(const std::array<float,4>& a, const std::array<float,4>& b);





