#include <jni.h>
#include <string>
#include <android/log.h>
#include <vector>
#include "tensorflow/lite/c/c_api.h"
#include "yolo_utils.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_tflitetest_MainActivity_testTFLiteRun(
        JNIEnv* env,
        jobject /* this */,
        jstring jmodelPath,
        jobject inputBuffer) {

    const char* model_path = env->GetStringUTFChars(jmodelPath, 0);
    TfLiteModel* model = TfLiteModelCreateFromFile(model_path);
    env->ReleaseStringUTFChars(jmodelPath, model_path);

    if (!model) return env->NewStringUTF("Load model failed");

    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);

    if (!interpreter) return env->NewStringUTF("Interpreter build failed");
    //分配 tensors
    if (TfLiteInterpreterAllocateTensors(interpreter) != kTfLiteOk) {
        TfLiteInterpreterDelete(interpreter);
        return env->NewStringUTF("AllocateTensors failed");
    }
    //取得輸入 tensor
    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    //從 Direct ByteBuffer 複製資料
    TfLiteTensorCopyFromBuffer(inputTensor,
                               env->GetDirectBufferAddress(inputBuffer),
                               TfLiteTensorByteSize(inputTensor));
    //推論
    if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk) {
        TfLiteInterpreterDelete(interpreter);
        return env->NewStringUTF("Inference failed");
    }

    //取得輸出
    const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
    const float* outputData = static_cast<const float*>(TfLiteTensorData(outputTensor));


    // 2. Extract BB
    int H=13, W=13, num_anchors=5, num_class=13;

    std::vector<std::array<float,4>> boxes;
    std::vector<std::vector<float>> class_probs;

    extract_bb(outputData, H, W, num_anchors, num_class, boxes, class_probs);
    auto detections = non_max_suppression(boxes, class_probs, 0.5f, 20);

    std::string result = "";
    for(auto& d:detections){
        result += "class=" + std::to_string(d.class_id) +
                  " prob=" + std::to_string(d.score) + "\n";
    }

    TfLiteInterpreterDelete(interpreter);
    return env->NewStringUTF(result.c_str());
}








