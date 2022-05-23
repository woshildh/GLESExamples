package com.example.glesexamples;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.app.ActivityManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ConfigurationInfo;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.glesexamples.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private String logTag = "[GLESExample]";
    // JNI 渲染器
    private JNIRender mRender;
    // GLSurfaceView
    private GLSurfaceView mSurfaceView;
    // 弹出框
    private AlertDialog.Builder mAlertDialog;
    // 渲染模式的选项
    int item_index = 0;
    // 两个按钮
    Button choose_btn;
    Button render_btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // 设置ContentView
        setContentView(R.layout.activity_main);
        mSurfaceView = findViewById(R.id.gl_view);
        // 初始化 render
        mRender = new JNIRender(this);
        if(detectedOGLES30() && mSurfaceView != null) {
            // 底下两行很重要，设置支持透明度和将视图放到最上面
            mSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            mSurfaceView.setZOrderOnTop(true);
            //
            mSurfaceView.setEGLContextClientVersion(3);
            mSurfaceView.setRenderer(mRender);
            // 设置为更新时进行渲染
            mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        } else {
            Log.e(logTag, "set render failed, mSurfaceView: " + mSurfaceView);
        }
        // 初始化弹出框
        final String items[] = {"三角形", "立方体", "多个立方体", "立方体纹理", "加载obj对象", "YUV图像", "图像交替", "下雪"};
        mAlertDialog = new AlertDialog.Builder(this);
        mAlertDialog.setTitle("选择模式").setSingleChoiceItems(items, 0, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                item_index = i;
            }
        });
        mAlertDialog.setPositiveButton("确认", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                mRender.render_type = item_index;
                Log.i(logTag, "渲染模式选择了: " + item_index);
            }
        });

        // 设置按钮的回调函数
        choose_btn = findViewById(R.id.choose_btn);
        render_btn = findViewById(R.id.render_btn);
        choose_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mAlertDialog.setSingleChoiceItems(items, item_index, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        item_index = i;
                        Log.i(logTag, "渲染模式选择了: " + i);
                    }
                });
                mAlertDialog.show();
            }
        });
        render_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

            }
        });
    }
    // 检查设备本地的opengles版本是不是3.0以上
    private boolean detectedOGLES30() {
        ActivityManager mng = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = mng.getDeviceConfigurationInfo();
        if(info.reqGlEsVersion >= 0x30000) {
            return true;
        }
        return false;
    }
}