//
// Created by ldh on 2022/4/2.
//

#include "CubeTextureRender.h"
#include "../utils/jpeg_decode.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CubeTextureRender::CubeTextureRender(AAssetManager *_mgr) {
    angle = 0;
    mgr = _mgr;
    // 定义程序
    vert_shader_src = "#version 300 es     \n"
                      "uniform mat4 transform_mat;    \n"
                      "layout(location = 0) in vec4 vert_pos;  \n"
                      "layout(location = 1) in vec2 tex_coord;  \n"
                      "out vec2 tc;    \n"
                      "void main()    \n"
                      "{   \n"
                      "    gl_Position = transform_mat * vert_pos;   \n"
                      "    tc = tex_coord;   \n"
                      "}";

    frag_shader_src = "#version 300 es     \n"
                      "layout(binding = 0) uniform sampler2D sampler;   \n"
                      "in vec2 tc;    \n"
                      "out vec4 outColor;    \n"
                      "void main()    \n"
                      "{   \n"
                      "    outColor = texture(sampler, tc);  \n"
                      "}";
    // 加载程序
    load_program();
    // 使用当前程序
    glUseProgram(program_id);
    trans_mat_loc = glGetUniformLocation(program_id, "transform_mat");
    // 打印调试
    LOG_I("[CubeTextureRender] program id: %d, vert shader: %d, frag shader: %d, trans_mat_loc: %d, opengl err: %d",
            program_id, vert_shader, frag_shader, trans_mat_loc, glGetError());
    // 初始化数据
    init_data();
}

CubeTextureRender::~CubeTextureRender() {
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_texture_id);
    glDeleteBuffers(1, &vbo_position_id);
    glDeleteTextures(1, &texture_id);
}

void CubeTextureRender::init_data() {
    // 顶点数据和纹理数据
    float pyramidPositions[54] =
            { -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
              1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
              1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
              -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
              -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
              1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
            };
    float textureCoordinates[36] =
            { 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
              0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
              0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
              0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
              0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
              1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
            };
    // 新建vao和vbo
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_position_id);
    glGenBuffers(1, &vbo_texture_id);
    // 发送数据到 vao 和 vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    // 读取图像
    AAsset* asset = AAssetManager_open(mgr, "brick.jpg", AASSET_MODE_UNKNOWN);
    off_t length = AAsset_getLength(asset);
    // 读取内存
    std::shared_ptr<char> buffer(new char[length + 2](), std::default_delete<char[]>());
    int len = AAsset_read(asset, buffer.get(), length);
    LOG_I("[CubeTextureRender] JPEG length is %d bytes, read %d bytes", length, len);
    // 解码成 rgb 数据
    Jpeg::Decoder decoder(reinterpret_cast<const unsigned char *>(buffer.get()), length);
    if(decoder.GetResult() != decoder.OK) {
        LOG_I("[CubeTextureRender] Parse image failed");
    }
    int height = decoder.GetHeight(), width = decoder.GetWidth();
    unsigned char* rgb_data = decoder.GetImage();
    // 关闭文件
    AAsset_close(asset);

    // 生成纹理数据
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // 打印
    LOG_I("[CubeTextureRender] load data opengl err: %d", glGetError());
}

void CubeTextureRender::render() {
    // 清除当前颜色和深度缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 计算平移旋转和投影矩阵
    angle += 0.01;
    glm::mat4 proj_mat = glm::perspective(1.0472f, width / (height + 0.01f), 0.1f, 1000.0f);
    glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0f));
    // 旋转矩阵
    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0), angle, glm::vec3(1.0, 0.0, 0.0));
    rotate_mat = glm::rotate(rotate_mat, angle * 1.2f, glm::vec3(0.0, 1.0, 0.0));
    rotate_mat = glm::rotate(rotate_mat, angle * 1.5f, glm::vec3(0.0, 0.0, 1.0));
    rotate_mat = proj_mat * trans_mat * rotate_mat;
    // 发送统一变量
    glUniformMatrix4fv(trans_mat_loc, 1, GL_FALSE, glm::value_ptr(rotate_mat));
    // 设置 vao 和 vbo 属性
    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // 允许深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 18);
}
