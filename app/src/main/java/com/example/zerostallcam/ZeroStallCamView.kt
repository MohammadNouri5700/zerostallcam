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

    override fun surfaceCreated(holder: SurfaceHolder) {
        nativeSurfaceCreated(holder.surface)
        nativeStartCamera()
        isRendering = true
        Choreographer.getInstance().postFrameCallback(frameCallback)
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
