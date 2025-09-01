package com.example.tflitetestsdk;

import android.content.Context;
import android.graphics.Bitmap;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class TflitetestSDK {
    static { System.loadLibrary("tflitetestsdk"); }

    // 讀取 assets 的 .tflite
    private static byte[] loadModelFile(Context context) {
        try (InputStream is = context.getAssets().open("model.tflite")) {
            byte[] buffer = new byte[is.available()];
            int read = is.read(buffer);
            if (read != buffer.length) throw new RuntimeException("Model file read error");
            return buffer;
        } catch (Exception e) {
            throw new RuntimeException("Failed to load model from assets", e);
        }
    }

    // 建立 detector
    public long yoloCreate(Context context) {
        byte[] modelBuffer = loadModelFile(context);
        return yoloCreateFromBuffer(modelBuffer);
    }

    public native long yoloCreateFromBuffer(byte[] modelBuffer);
    public native void yoloDestroy(long handle);
    public native String yoloRun(long handle, ByteBuffer inputBuffer);

    // 將 Bitmap 轉 FloatBuffer
    public static ByteBuffer bitmapToInputBuffer(Bitmap bitmap, int inputSize) {
        Bitmap resized = Bitmap.createScaledBitmap(bitmap, inputSize, inputSize, true);
        ByteBuffer buffer = ByteBuffer.allocateDirect(inputSize * inputSize * 3 * 4)
                .order(ByteOrder.nativeOrder());
        int[] pixels = new int[inputSize * inputSize];
        resized.getPixels(pixels, 0, inputSize, 0, 0, inputSize, inputSize);
        for (int px : pixels) {
            buffer.putFloat(((px >> 16) & 0xFF) / 255.f); // R
            buffer.putFloat(((px >> 8) & 0xFF) / 255.f);  // G
            buffer.putFloat((px & 0xFF) / 255.f);         // B
        }
        buffer.rewind();
        return buffer;
    }
}

