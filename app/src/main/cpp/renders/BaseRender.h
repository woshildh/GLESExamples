//
// Created by ldh on 2022/3/29.
//

#ifndef GLESEXAMPLES_BASERENDER_H
#define GLESEXAMPLES_BASERENDER_H

#include <string>
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "[GLESExample]"
#define LOG_I(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


class BaseRender {
public:
    // 构造函数，在构造函数内初始化自己的变量等
    BaseRender();
    // 加载程序
    void load_program();
    // 设置宽高
    void resize_window(int w, int h);
    // 进行渲染
    virtual void render() = 0;
    // 析构函数
    virtual ~BaseRender();

protected:
    // 顶点着色器源代码
    std::string vert_shader_src;
    // 片段着色器源代码
    std::string frag_shader_src;
    // 顶点着色器程序
    GLuint vert_shader;
    // 片段着色器程序
    GLuint frag_shader;
    // 程序 id
    GLuint program_id;
    // 宽和高
    int width;
    int height;
};


#endif //GLESEXAMPLES_BASERENDER_H
