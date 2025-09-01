#include <jni.h>
#include <string>
#include "yolo_detector.h"
#include "yolo_utils.h"

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_example_tflitetestsdk_TflitetestSDK_yoloCreateFromBuffer(JNIEnv* env, jobject thiz, jbyteArray modelBuffer) {
    jsize length = env->GetArrayLength(modelBuffer);
    jbyte* buffer = env->GetByteArrayElements(modelBuffer, nullptr);

    TfLiteModel* model = TfLiteModelCreate(static_cast<const void*>(buffer), static_cast<size_t>(length));
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);

    env->ReleaseByteArrayElements(modelBuffer, buffer, JNI_ABORT);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);

    auto* detector = new YoloDetector(interpreter);
    return reinterpret_cast<jlong>(detector);
}

JNIEXPORT void JNICALL
Java_com_example_tflitetestsdk_TflitetestSDK_yoloDestroy(JNIEnv* env, jobject thiz, jlong handle) {
    auto* detector = reinterpret_cast<YoloDetector*>(handle);
    delete detector;
}

JNIEXPORT jstring JNICALL
Java_com_example_tflitetestsdk_TflitetestSDK_yoloRun(JNIEnv* env, jobject thiz, jlong handle, jobject inputBuffer) {
    auto* detector = reinterpret_cast<YoloDetector*>(handle);
    float* buffer = reinterpret_cast<float*>(env->GetDirectBufferAddress(inputBuffer));

    auto detections = detector->run(buffer);
    std::string result;
    for (auto& d : detections) {
        result += "class=" + std::to_string(d.class_id) +
                  " prob=" + std::to_string(d.score) + "\n";
    }
    return env->NewStringUTF(result.c_str());
}

}


