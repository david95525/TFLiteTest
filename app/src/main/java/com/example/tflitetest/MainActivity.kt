package com.example.tflitetest

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import java.io.File
import java.io.FileOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import com.example.tflitetest.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    companion object {
        init { System.loadLibrary("tflitetest") }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // 複製 model 到內部存儲
        val modelFile = File(filesDir, "model.tflite")
        if (!modelFile.exists()) {
            assets.open("model.tflite").use { input ->
                FileOutputStream(modelFile).use { output ->
                    input.copyTo(output)
                }
            }
        }

        val bitmap = BitmapFactory.decodeStream(assets.open("test.jpg"))
        //  確保 bitmap 尺寸為 416x416
        val resizedBitmap = Bitmap.createScaledBitmap(bitmap, 416, 416, true)
        //  建立 direct ByteBuffer
        val inputBuffer = ByteBuffer.allocateDirect(1 * 416 * 416 * 3 * 4)
        inputBuffer.order(ByteOrder.nativeOrder())
        //  將 bitmap 轉 float，寫入 buffer
        for (y in 0 until 416) {
            for (x in 0 until 416) {
                val pixel = resizedBitmap.getPixel(x, y)
                inputBuffer.putFloat(((pixel shr 16) and 0xFF) / 255f) // R
                inputBuffer.putFloat(((pixel shr 8) and 0xFF) / 255f)  // G
                inputBuffer.putFloat((pixel and 0xFF) / 255f)           // B
            }
        }
        //重置 position 才能傳給 JNI
        inputBuffer.rewind()
        // 呼叫 JNI 推論
        binding.resultText.text = testTFLiteRun(modelFile.absolutePath, inputBuffer)
    }
    // 將 Bitmap 轉成模型輸入 ByteBuffer
    private fun bitmapToFloatBuffer(bitmap: Bitmap): ByteBuffer {
        val inputSize = 416
        val img = Bitmap.createScaledBitmap(bitmap, inputSize, inputSize, true)
        val byteBuffer = ByteBuffer.allocateDirect(4 * inputSize * inputSize * 3)
        byteBuffer.order(ByteOrder.nativeOrder())
        val intValues = IntArray(inputSize * inputSize)
        img.getPixels(intValues, 0, inputSize, 0, 0, inputSize, inputSize)
        for (pixel in intValues) {
            val r = ((pixel shr 16) and 0xFF) / 255.0f
            val g = ((pixel shr 8) and 0xFF) / 255.0f
            val b = (pixel and 0xFF) / 255.0f
            byteBuffer.putFloat(r)
            byteBuffer.putFloat(g)
            byteBuffer.putFloat(b)
        }
        byteBuffer.rewind()
        return byteBuffer
    }
    external fun testTFLiteRun(modelPath: String, inputBuffer: ByteBuffer): String
}






