#include "yolo_utils.h"

float logistic(float x) { return 1.f / (1.f + std::exp(-x)); }

static const std::vector<std::array<float,2>> YOLO_ANCHORS = {
        {0.573f, 0.677f},
        {1.87f, 2.06f},
        {3.34f, 5.47f},
        {7.88f, 3.53f},
        {9.77f, 9.17f}
};

void softmax(float* data, int length) {
    float max_val = *std::max_element(data, data+length);
    float sum = 0.f;
    for (int i=0;i<length;i++){
        data[i] = std::exp(data[i]-max_val);
        sum += data[i];
    }
    for (int i=0;i<length;i++) data[i] /= sum;
}

void extract_bb(const float* prediction_output, int H, int W, int num_anchors, int num_class,
                std::vector<std::array<float,4>>& boxes,
                std::vector<std::vector<float>>& class_probs) {
    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
            for (int a=0; a<num_anchors; a++){
                int idx = (i*W*num_anchors + j*num_anchors + a)*(num_class+5);

                float x = logistic(prediction_output[idx + 0]);
                float y = logistic(prediction_output[idx + 1]);
                float w = std::exp(prediction_output[idx + 2]) * YOLO_ANCHORS[a][0];
                float h = std::exp(prediction_output[idx + 3]) * YOLO_ANCHORS[a][1];
                float objectness = logistic(prediction_output[idx + 4]);

                std::vector<float> cls(num_class);
                for (int c=0;c<num_class;c++) cls[c] = prediction_output[idx+5+c];
                softmax(cls.data(), num_class);

                for(int c=0;c<num_class;c++) cls[c] *= objectness;

                boxes.push_back({x - w/2, y - h/2, w, h});
                class_probs.push_back(cls);
            }
        }
    }
}

float iou(const std::array<float,4>& a, const std::array<float,4>& b){
    float x1 = std::max(a[0], b[0]);
    float y1 = std::max(a[1], b[1]);
    float x2 = std::min(a[0]+a[2], b[0]+b[2]);
    float y2 = std::min(a[1]+a[3], b[1]+b[3]);
    float w = std::max(0.f, x2-x1);
    float h = std::max(0.f, y2-y1);
    float inter = w*h;
    float union_area = a[2]*a[3] + b[2]*b[3] - inter;
    return inter/union_area;
}

std::vector<Detection> non_max_suppression(const std::vector<std::array<float,4>>& boxes,
                                           const std::vector<std::vector<float>>& class_probs,
                                           float prob_threshold, int max_detections){
    std::vector<Detection> dets;

    for (size_t i=0;i<boxes.size();i++){
        auto& probs = class_probs[i];
        float max_prob = *std::max_element(probs.begin(), probs.end());
        int class_id = std::distance(probs.begin(), std::max_element(probs.begin(), probs.end()));
        if(max_prob < prob_threshold) continue;
        dets.push_back({boxes[i], class_id, max_prob});
    }

    std::sort(dets.begin(), dets.end(), [](const Detection& a, const Detection& b){
        return a.score > b.score;
    });

    std::vector<Detection> selected;
    while(!dets.empty() && selected.size() < (size_t)max_detections){
        selected.push_back(dets[0]);
        std::vector<Detection> rest;
        for(size_t i=1;i<dets.size();i++){
            if(iou(selected.back().box, dets[i].box) <= 0.45f) rest.push_back(dets[i]);
        }
        dets = rest;
    }

    return selected;
}

