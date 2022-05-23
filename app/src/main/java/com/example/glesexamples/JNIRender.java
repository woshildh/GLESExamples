package com.example.glesexamples;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

// 用于通过 JNI 调用底层 c++
public class JNIRender implements GLSurfaceView.Renderer {
    static {
        System.loadLibrary("glesexamples");
    }
    public int render_type;

    // 初始化 NDK render
    public native void initRender(AssetManager mgr);

    // 设置使用的渲染器类型
    public native void setRenderType(int type);

    // 设置窗口的宽和高
    public native void setWindowSize(int width, int height);

    // 进行渲染
    public native void renderWindow();

    private AssetManager mAssetMgr = null;
    private String mLogTag = "[GLESExample]";
    public JNIRender(Context ctx) {
        mAssetMgr = ctx.getAssets();
        if(mAssetMgr == null) {
            Log.e(mLogTag, "JNIRender: getAssests() failed.");
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        initRender(mAssetMgr);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        setWindowSize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        setRenderType(render_type);
        renderWindow();
    }
}
