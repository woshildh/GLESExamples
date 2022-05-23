//
// Created by ldh on 2022/4/4.
//

#include "YUVImageRender.h"
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

YUVImageRender::YUVImageRender(AAssetManager *_mgr, int _w, int _h) {
    // 赋初始值
    mgr = _mgr;
    width = _w, height = _h;
    vao_id = vbo_id = trans_mat_loc  = -1;
    texture_loc[0] = texture_loc[1] = texture_loc[2] = -1;
    trans_z = 1.0, positive = 1;
    // 设置着色器源码
    vert_shader_src = "#version 300 es   \n"
                      "layout(location = 0) in vec4 verts_info;   \n"
                      "uniform mat4 transform_mat;  \n"
                      "out vec2 tex_coord;   \n"
                      "void main()  \n"
                      "{   \n"
                      "    gl_Position = transform_mat * vec4(verts_info.x, verts_info.y, 0.0, 1.0);  \n"
                      "    tex_coord = vec2(verts_info.z, verts_info.w);  \n"
                      "}";

    frag_shader_src = "#version 300 es     \n"
                      "layout(binding = 0) uniform sampler2D samp_y; \n"
                      "layout(binding = 1) uniform sampler2D samp_u; \n"
                      "layout(binding = 2) uniform sampler2D samp_v; \n"
                      "in vec2 tex_coord;   \n"
                      "out vec4 frag_color;  \n"
                      "void main()  \n"
                      "{    \n"
                      "    vec3 yuv;  \n"
                      "    yuv.r = texture(samp_y, tex_coord).r;   \n"
                      "    yuv.g = texture(samp_u, tex_coord).r - 0.5;   \n"
                      "    yuv.b = texture(samp_v, tex_coord).r - 0.5;    \n"
                      "    vec3 rgb = mat3(    \n"
                      "        1, 1, 1,\n"
                      "        0, -0.344, 1.770,\n"
                      "        1.403, -0.714, 0) * yuv;  \n"
                      "    frag_color = vec4(rgb, 1.0);   \n"
                      "}";
    // 加载程序
    load_program();
    // 初始化数据
    init_data();
    // 打印调试
    LOG_I("[YUVImageRender] program id: %d, vert shader: %d, frag shader: %d, trans_mat_loc: %d, opengl err: %d",
          program_id, vert_shader, frag_shader, trans_mat_loc, glGetError());

}

YUVImageRender::~YUVImageRender() noexcept {
    glDeleteTextures(3, texture_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
}

void YUVImageRender::calculate_vertices(float *vertices) {
    LOG_I("calculate_vertices width: %d, height: %d", this->width, this->width);
    // 计算图像和窗口的宽高比
    float win_wh_ratio = (float)width / height;
    float img_wh_ratio = (float)YUV_IMAGE_WIDTH / (float)YUV_IMAGE_HEIGHT;
    // 确定宽和高
    float new_width = 2, new_height = 2;
    if(win_wh_ratio < img_wh_ratio) {
        new_width = 2.0;
        new_height = 2 * (width / img_wh_ratio) / height;
    } else {
        new_height = 2.0;
        new_width = 2 * (height * img_wh_ratio) / width;
    }
    LOG_I("[YUVImageRender] height: %d, width: %d, win_wh_ratio: %f, img_wh_ratio: %f, width: %f, height: %f",
          height, width, win_wh_ratio, img_wh_ratio, new_width, new_height);
    // 确定顶点坐标
    float tmp[16] = {
            -new_width / 2.0f, new_height / 2.0f, 0.0, 0.0,   // 左上角
            -new_width / 2.0f, -new_height / 2.0f, 0.0, 1.0,   // 左下角
            new_width / 2.0f,  -new_height / 2.0f, 1.0, 1.0,   // 右下角
            new_width / 2.0f, new_height / 2.0f, 1.0, 0.0      // 右上角
    };
    memcpy(vertices, tmp, sizeof(float) * 16);
}

NV21Image* YUVImageRender::read_yuv() {
    // 打开aaset
    AAsset* asset = AAssetManager_open(mgr, YUV_FILE_NAME, AASSET_MODE_UNKNOWN);
    // 获取长度
    off_t length = AAsset_getLength(asset);
    // 分配内存
    unsigned char *buffer = (unsigned char*) malloc(length + 1);
    // 读取
    AAsset_read(asset, buffer, length);
    // 关闭
    AAsset_close(asset);
    // 新建一个 NV21Image
    NV21Image *image = new NV21Image(buffer, YUV_IMAGE_WIDTH, YUV_IMAGE_HEIGHT);
    // 记得释放内存
    free(buffer);
    return image;
}

void YUVImageRender::init_data() {
    // 使用当前程序
    glUseProgram(program_id);
    // 获取统一变量的位置
    texture_loc[0] = glGetUniformLocation(program_id, "samp_y");
    texture_loc[1] = glGetUniformLocation(program_id, "samp_u");
    texture_loc[2] = glGetUniformLocation(program_id, "samp_v");
    trans_mat_loc  = glGetUniformLocation(program_id, "transform_mat");
    LOG_I("[YUVImageRender] texture_loc: %d, %d, %d, trans_mat_loc: %d",
          texture_loc[0], texture_loc[1], texture_loc[2], trans_mat_loc);
    // 生成vao 和 vbo, 并发送顶点数据
    float vertices[16];
    calculate_vertices(vertices);
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 读取数据
    NV21Image *image = read_yuv();
    LOG_I("Read NV21 succeed");
    // 生成纹理
    glGenTextures(3, texture_id);
    // 设置 Y 纹理属性
    {
        glBindTexture(GL_TEXTURE_2D, texture_id[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, YUV_IMAGE_WIDTH, YUV_IMAGE_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, image->y_plane);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // 设置 u 纹理属性
    {
        glBindTexture(GL_TEXTURE_2D, texture_id[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, YUV_IMAGE_WIDTH / 2, YUV_IMAGE_HEIGHT / 2, 0, GL_RED, GL_UNSIGNED_BYTE, image->u_plane);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // 设置 v 纹理属性
    {
        glBindTexture(GL_TEXTURE_2D, texture_id[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, YUV_IMAGE_WIDTH / 2, YUV_IMAGE_HEIGHT / 2, 0, GL_RED, GL_UNSIGNED_BYTE, image->v_plane);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    // 记得释放数据
    delete image;
}

void YUVImageRender::render() {
    if(positive == 1) {
        trans_z -= 0.005;
    } else {
        trans_z += 0.005;
    }
    if(trans_z < 0.1) {
        positive = -1;
    } else if(trans_z > 1.0) {
        positive = 1;
    }
    trans_z = fmax(fmin(trans_z, 1.0f), 0.1f);
    // 设置视口
    glViewport(0.0, 0.0, this->width, this->height);
    // 清除当前颜色缓冲区和深度缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置 顶点属性
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    glUniform1i(texture_loc[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    glUniform1i(texture_loc[1], 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    glUniform1i(texture_loc[2], 2);
    // 发送统一变量
//    glm::mat4 pers_mat = glm::perspective(0.5f, (float)width / height, 0.1f, 20.0f);
    glm::mat4 pers_mat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    glm::mat4 trans_mat = glm::scale(glm::mat4(1.0f), glm::vec3(trans_z, trans_z, 1.0f));
    trans_mat = pers_mat * trans_mat;
    glUniformMatrix4fv(trans_mat_loc, 1, GL_FALSE, glm::value_ptr(trans_mat));
    // 设置elements
    GLushort indices[6] = {0, 1, 2, 0, 2, 3};
    // 进行绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}