//
// Created by ldh on 2022/4/4.
//

#include "ImageChangeRender.h"
#include <math.h>

ImageChangeRender::ImageChangeRender(AAssetManager *_mgr) {
    mgr = _mgr;
    // 初始化值
    vao_id = vbo_id = 0;
    cross_height = -1.0;
    positive = 1.0;
    // 定义着色器语言值
    vert_shader_src = "#version 300 es       \n"
                      "uniform float cross_height;  \n"
                      "layout(location = 0) in vec4 vert_info;  \n"
                      "out vec2 tex_coord;     \n"
                      "out float vert_height;  \n"
                      "void main()   \n"
                      "{  \n"
                      "    gl_Position = vec4(vert_info.x, vert_info.y, 0.0, 1.0f);  \n"
                      "    vert_height = vert_info.y;   \n"
                      "    tex_coord = vec2(vert_info.z, vert_info.w);  \n"
                      "}";
    frag_shader_src = "#version 300 es       \n"
                      "layout(binding = 0) uniform sampler2D samp1;   \n"
                      "layout(binding = 1) uniform sampler2D samp2;   \n"
                      "uniform float cross_height;"
                      "in vec2 tex_coord;   \n"
                      "in float vert_height;"
                      "out vec4 out_color;   \n"
                      "void main()  \n"
                      "{ \n"
                      "   if(cross_height <= vert_height) \n"
                      "        out_color = texture(samp1, tex_coord);  \n"
                      "    else \n"
                      "        out_color = texture(samp2, tex_coord);   \n"
                      "}";
    // 加载程序
    load_program();
    // 初始化数据
    init_data();
    LOG_I("[YUVImageRender] program id: %d, vert shader: %d, frag shader: %d, cross_height_loc: %d, opengl err: %d",
          program_id, vert_shader, frag_shader, cross_height_loc, glGetError());
}

Jpeg::Decoder * ImageChangeRender::read_jpeg(std::string jpeg_path) {
    // 打开aaset
    AAsset* asset = AAssetManager_open(mgr, jpeg_path.c_str(), AASSET_MODE_UNKNOWN);
    // 获取长度
    off_t length = AAsset_getLength(asset);
    // 分配内存
    unsigned char *buffer = (unsigned char*) malloc(length + 1);
    // 读取
    AAsset_read(asset, buffer, length);
    // 关闭
    AAsset_close(asset);
    // 新建一个 NV21Image
    Jpeg::Decoder *decoder = new Jpeg::Decoder(buffer, length);
    // 记得释放内存
    free(buffer);
    LOG_I("[ImageChangeRender] Jpeg state: %d, size: (%d, %d)", decoder->GetResult(), decoder->GetWidth(), decoder->GetHeight());
    // 返回结果
    return decoder;
}

void ImageChangeRender::init_data() {
    // 使用当前程序
    glUseProgram(program_id);
    // 获取统一变量的位置
    texture_uni_loc[0] = glGetUniformLocation(program_id, "samp1");
    texture_uni_loc[1] = glGetUniformLocation(program_id, "samp2");
    cross_height_loc = glGetUniformLocation(program_id, "cross_height");
    // 生成vao 和 vbo, 并发送顶点数据
    float vertices[24] = {
         -1.0f, 1.0f, 0.0f, 0.0f,   // 左上
         -1.0f, -1.0f, 0.0f, 1.0f,   // 左下
         1.0f, -1.0f, 1.0f, 1.0f,   // 右下
         -1.0f, 1.0f, 0.0f, 0.0f,   // 左上
         1.0f, -1.0f, 1.0f, 1.0f,  // 右下
         1.0f, 1.0f, 1.0f, 0.0f   // 右上
    };
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 读取Jpeg图像数据
    Jpeg::Decoder *decoder1 = read_jpeg(IMAGE1_NAME);
    Jpeg::Decoder *decoder2 = read_jpeg(IMAGE2_NAME);
    // 生成纹理
    glGenTextures(2, texture_id);
    // 对两个纹理进行设置和传送数据
    {
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, decoder1->GetWidth(), decoder1->GetHeight(),
                     0, GL_RGB, GL_UNSIGNED_BYTE, decoder1->GetImage());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    {
        glBindTexture(GL_TEXTURE_2D, texture_id[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, decoder2->GetWidth(), decoder2->GetHeight(),
                     0, GL_RGB, GL_UNSIGNED_BYTE, decoder2->GetImage());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    LOG_I("[ImageChangeRender] texture id: %d, %d, loc: %d, %d, err: %d",
          texture_id[0], texture_id[1], texture_uni_loc[0], texture_uni_loc[1], glGetError());
    // 释放相应的图像解码器
    delete decoder1;
    delete decoder2;
}

ImageChangeRender::~ImageChangeRender() {
    glDeleteBuffers(1, &vbo_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteTextures(2, texture_id);
}

void ImageChangeRender::render() {
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置cross_height的位置
    cross_height += (0.005 * positive);
    if(cross_height > 1.0) {
        positive = -1;
    } else if(cross_height < -1.0) {
        positive = 1.0;
    }
    cross_height = fmin(fmax(-1.0, cross_height), 1.0);
    // 发送统一变量
    glUniform1f(cross_height_loc, cross_height);
    // 设置 vao 属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    // 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    glUniform1i(texture_uni_loc[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    glUniform1i(texture_uni_loc[1], 1);

    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
