package com.example.tflitetest;

import java.nio.FloatBuffer;
public class TflitetestSDK {
    static { System.loadLibrary("tflitetestsdk"); }
    public native long yoloCreate(String modelPath);
    public native void yoloDestroy(long handle);
    public native String yoloRun(long handle, FloatBuffer inputBuffer);
}