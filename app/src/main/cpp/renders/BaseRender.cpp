//
// Created by ldh on 2022/3/29.
//

#include "BaseRender.h"
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <stdlib.h>


using namespace std;

// 创建 shader
GLuint create_shader(GLenum type, string src) {
    if(src.empty()) {
        LOG_E("Shader src is empty");
        return 0;
    }
    // 创建shader
    GLuint shader = glCreateShader(type);
    // 加载源码
    const char *src_str = src.c_str();
    glShaderSource(shader, 1, &src_str, NULL);
    // 编译
    glCompileShader(shader);
    // 检查 shader 状态
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if(length) {
            char *info = (char*) malloc(sizeof(char) * length);
            glGetShaderInfoLog(shader, length, &length, info);
            LOG_E("Compile shader failed. %s", info);
            free(info);
        }
        glDeleteShader(shader);
    }
    return shader;
}

BaseRender::BaseRender() {

}

void BaseRender::load_program() {
    // 加载vert_shader
    vert_shader = create_shader(GL_VERTEX_SHADER, vert_shader_src);
    LOG_I("vert_shader: %d, state: %d", vert_shader, glGetError());
    // 加载 frag_shader
    frag_shader = create_shader(GL_FRAGMENT_SHADER, frag_shader_src);
    LOG_I("frag_shader: %d, state: %d", frag_shader, glGetError());
    // 创建程序
    program_id = glCreateProgram();
    LOG_I("create: %d, state: %d", program_id, glGetError());

    // 绑定着色器
    glAttachShader(program_id, vert_shader);
    LOG_I("attch 1: %d, state: %d", program_id, glGetError());
    glAttachShader(program_id, frag_shader);
    LOG_I("attch 2: %d, state: %d", program_id, glGetError());

    // 连接程序
    glLinkProgram(program_id);
    LOG_I("link program: %d, state: %d", program_id, glGetError());
    // 检查状态
    GLint linked = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLint length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
        if(length > 0) {
            char *info = (char*) malloc(sizeof(char) * length);
            glGetProgramInfoLog(program_id, length, NULL, info);
            LOG_E("Link program failed. %s", info);
            free(info);
        }
        glDeleteProgram(program_id);
    }
}

void BaseRender::resize_window(int w, int h) {
    width = w;
    height = h;
    LOG_I("Now window size: (%d, %d)", width, height);
}

BaseRender::~BaseRender() {
    if(vert_shader) {
        glDeleteShader(vert_shader);
    }
    if(frag_shader) {
        glDeleteShader(frag_shader);
    }
    if(program_id) {
        glDeleteProgram(program_id);
    }
}

