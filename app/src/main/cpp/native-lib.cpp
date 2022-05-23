#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include "renders/TriangleRender.h"
#include "renders/CubeRender.h"
#include "renders/MultiCubeRender.h"
#include "renders/CubeTextureRender.h"
#include "renders/YUVImageRender.h"
#include "renders/ImageChangeRender.h"
#include "renders/ModelShow.h"
#include "renders/SnowRender.h"

#define LOG_TAG "[GLESExample]"
#define LOG_I(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


// 定义窗口的宽高
size_t g_width, g_height;

// 全局的AssetManager
AAssetManager *g_asset_mgr = NULL;

// 渲染器类型
int render_type = 1;

// 全局的渲染器
BaseRender *g_render = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_glesexamples_JNIRender_initRender(JNIEnv *env, jobject thiz, jobject mgr) {
    if(env && mgr) {
        g_asset_mgr = AAssetManager_fromJava(env, mgr);
        if(g_asset_mgr == NULL) {
            LOG_E("AAssetManager_fromJava() is NULL!");
        }
    } else {
        LOG_E("[readShaderFile] env or mgr is NULL!");
    }
    g_render = new TriangleRender();
    LOG_I("Render load succeed");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_glesexamples_JNIRender_setRenderType(JNIEnv *env, jobject thiz, jint type) {
    // 如果两张类型一致直接返回
    if(render_type == type) {
        return;
    }
    render_type = type;
    if(g_render) {
        delete g_render;
        g_render = NULL;
    }
    if(render_type == 0) {
        g_render = new TriangleRender();
    } else if(render_type == 1) {
        g_render = new CubeRender();
    } else if(render_type == 2) {
        g_render  = new MultiCubeRender();
    } else if(render_type == 3) {
        g_render = new CubeTextureRender(g_asset_mgr);
    }  else if(render_type == 4) {
        g_render = new ModelShow(g_asset_mgr);
    } else if(render_type == 5) {
        g_render = new YUVImageRender(g_asset_mgr, g_width, g_height);
    } else if(render_type == 6) {
        g_render = new ImageChangeRender(g_asset_mgr);
    } else if(render_type == 7) {
        g_render = new SnowRender(g_asset_mgr);
    }
    if(g_render != NULL) {
        g_render->resize_window(g_width, g_height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_glesexamples_JNIRender_setWindowSize(JNIEnv *env, jobject thiz, jint width,
                                                      jint height) {
    g_width = width;
    g_height = height;
    if(g_render != NULL) {
        g_render->resize_window(g_width, g_height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_glesexamples_JNIRender_renderWindow(JNIEnv *env, jobject thiz) {
    if(g_render)
        g_render->render();
}
