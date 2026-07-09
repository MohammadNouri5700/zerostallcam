package com.example.zerostallcam

import android.content.Context
import android.util.AttributeSet
import android.view.Choreographer
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class ZeroStallCamView(context: Context, attrs: AttributeSet) : SurfaceView(context, attrs), SurfaceHolder.Callback {

    private var isRendering = false

    init {
        holder.addCallback(this)
    }

    companion object {
        init {
            System.loadLibrary("zerostallcam")
        }
    }

    private external fun nativeSurfaceCreated(surface: Surface)
    private external fun nativeSurfaceDestroyed()
    private external fun nativeStartCamera()
    private external fun nativeDrawFrame()
    private external fun nativeSetFontAtlas(bitmap: android.graphics.Bitmap)

    override fun surfaceCreated(holder: SurfaceHolder) {
        nativeSurfaceCreated(holder.surface)
        initHDTimestamp()
        nativeStartCamera()
        isRendering = true
        Choreographer.getInstance().postFrameCallback(frameCallback)
    }

    private fun initHDTimestamp() {
        val bitmap = android.graphics.Bitmap.createBitmap(1100, 100, android.graphics.Bitmap.Config.ARGB_8888)
        val canvas = android.graphics.Canvas(bitmap)
        val paint = android.graphics.Paint().apply {
            color = android.graphics.Color.WHITE
            textSize = 80f
            isAntiAlias = true
            typeface = android.graphics.Typeface.create(android.graphics.Typeface.DEFAULT, android.graphics.Typeface.BOLD)
        }
        val text = "0123456789:"
        for (i in text.indices) {
            canvas.drawText(text[i].toString(), i * 100f + 10f, 80f, paint)
        }
        nativeSetFontAtlas(bitmap)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        isRendering = false
        nativeSurfaceDestroyed()
    }

    private val frameCallback = object : Choreographer.FrameCallback {
        override fun doFrame(frameTimeNanos: Long) {
            if (isRendering) {
                nativeDrawFrame()
                Choreographer.getInstance().postFrameCallback(this)
            }
        }
    }
}
