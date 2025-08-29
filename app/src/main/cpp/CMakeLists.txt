# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
project("tflitetest")

# native-lib
add_library(
        tflitetestsdk
        SHARED
        yolo_utils.cpp
        yolo_detector.cpp
        yolo_jni.cpp)
# 系統庫
find_library(log-lib log)
find_library(android-lib android)

# TensorFlow Lite C API (IMPORTED)
add_library(tflite-lib SHARED IMPORTED)
set_target_properties(tflite-lib PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libtensorflowlite_c.so
)

# include
include_directories(
        ${CMAKE_SOURCE_DIR}/tflite_include
        ${CMAKE_SOURCE_DIR}/flatbuffers_include
)

# link
target_link_libraries(
        tflitetestsdk
        ${log-lib}
        ${android-lib}
        tflite-lib
)


