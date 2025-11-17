package com.example.tflitetest

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.tflitetestsdk.TflitetestSDK
import com.example.tflitetest.databinding.ActivityMainBinding
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
//react native sdk test
import com.facebook.react.ReactInstanceManager
import com.facebook.react.ReactNativeHost
import com.facebook.react.ReactRootView
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.modules.core.DefaultHardwareBackBtnHandler

class MainActivity : AppCompatActivity() {
    private lateinit var reactInstanceManager: ReactInstanceManager
    private lateinit var reactContext: ReactApplicationContext

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 初始化 ReactInstanceManager
        reactInstanceManager = ReactInstanceManager.builder()
            .setApplication(application)
            .setCurrentActivity(this)
            .setBundleAssetName("index.android.bundle") // 你的 bundle
            .setJSMainModulePath("index") // 對應 index.ts
            .addPackage(RNOnnxLibraryPackage()) // 你的 package
            .setUseDeveloperSupport(false)
            .setInitialLifecycleState(LifecycleState.RESUMED)
            .build()

        reactContext = ReactApplicationContext(this)
    }

    private fun runOnnxPrediction(imagePath: String) {
        val module = reactContext.getNativeModule(RNOnnxLibraryModule::class.java)
        module.runPrediction(imagePath, object : Promise {
            override fun resolve(value: Any?) {
                Log.d("ONNX", "CallbackId: $value")
            }
            override fun reject(code: String?, message: String?, e: Throwable?) {}
            override fun reject(code: String?, e: Throwable?) {}
            override fun reject(e: Throwable?) {}
            override fun reject(code: String?, message: String?) {}
        })

        // 監聽 JS 回傳結果
        val emitter = reactContext.getJSModule(DeviceEventManagerModule.RCTDeviceEventEmitter::class.java)
        emitter.addListener("PredictionResultEvent") { data ->
            Log.d("ONNX", "Result: ${data["result"]}")
        }
    }
//    private lateinit var binding: ActivityMainBinding
//    private lateinit var sdk: TflitetestSDK
//    private var handle: Long = 0L

//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//        binding = ActivityMainBinding.inflate(layoutInflater)
//        setContentView(binding.root)
//
//        // 1️⃣ 初始化 SDK（不需要傳 model path）
//        sdk = TflitetestSDK()
//        handle = sdk.yoloCreate(null)
//        // JNI 內部會自動從 assets 載入 model
//
//        // 2️⃣ 讀取圖片並轉成 FloatBuffer
//        val bitmap = BitmapFactory.decodeStream(assets.open("test.jpg"))
//        val floatBuffer = bitmapToFloatBuffer(bitmap)
//        val byteBuffer = floatBufferToByteBuffer(floatBuffer)
//        // 3️⃣ 呼叫 SDK 推論
//        val result = sdk.yoloRun(handle, byteBuffer)
//        binding.resultText.text = result
//    }

//    override fun onDestroy() {
//        super.onDestroy()
//        // 4️⃣ 釋放 SDK 資源
//        if (handle != 0L) {
//            sdk.yoloDestroy(handle)
//            handle = 0L
//        }
//    }

    // 將 Bitmap 轉成 FloatBuffer
//    private fun bitmapToFloatBuffer(bitmap: Bitmap): FloatBuffer {
//        val inputSize = 416
//        val img = Bitmap.createScaledBitmap(bitmap, inputSize, inputSize, true)
//        val floatBuffer = ByteBuffer.allocateDirect(4 * inputSize * inputSize * 3)
//            .order(ByteOrder.nativeOrder())
//            .asFloatBuffer()
//        val intValues = IntArray(inputSize * inputSize)
//        img.getPixels(intValues, 0, inputSize, 0, 0, inputSize, inputSize)
//        for (pixel in intValues) {
//            floatBuffer.put(((pixel shr 16) and 0xFF) / 255.0f) // R
//            floatBuffer.put(((pixel shr 8) and 0xFF) / 255.0f)  // G
//            floatBuffer.put((pixel and 0xFF) / 255.0f)           // B
//        }
//        floatBuffer.rewind()
//        return floatBuffer
//    }
//    private fun floatBufferToByteBuffer(floatBuffer: FloatBuffer): ByteBuffer {
//        val bb = ByteBuffer.allocateDirect(floatBuffer.capacity() * 4) // 每個 float 4 bytes
//            .order(ByteOrder.nativeOrder())
//        floatBuffer.rewind()
//        bb.asFloatBuffer().put(floatBuffer)
//        return bb
//    }
}
