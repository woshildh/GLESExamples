//
// Created by ldh on 2022/4/1.
//

#include "MultiCubeRender.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdlib.h>
#include <random>

MultiCubeRender::MultiCubeRender() {
    // 设置初始值
    vao_id = vbo_id = transform_mat_loc = 0;
    angle = 0.0;
    // 设置顶点着色器和片段着色器的值
    vert_shader_src = "#version 300 es     \n"
                      "layout(location = 0) in vec4 position;  \n"
                      "uniform mat4 transform_mat;  \n"
                      "out vec4 outPosition;   \n"
                      "void main()    \n"
                      "{  \n"
                      "    gl_Position = transform_mat * position;  \n"
                      "    outPosition = position;   \n"
                      "}  \n";
    frag_shader_src = "#version 300 es     \n"
                      "out vec4 outColor;   \n"
                      "in vec4 outPosition;  \n"
                      "void main()     \n"
                      "{   \n"
                      "    outColor = vec4(outPosition.x, outPosition.y, 0.42, 1.0);"
                      "}";
    // 加载程序
    load_program();
    // 打印输出
    LOG_I("[MultiCubeRender] program id: %d, vert_shader: %d, frag_shader: %d, opengl state: %d",
          program_id, vert_shader, frag_shader, glGetError());
    // 使用当前程序
    glUseProgram(program_id);
    // 初始化 vao 和 vbo 以及 transform_mat_loc
    init_vertices();
}

void MultiCubeRender::init_vertices() {
    // 生成 vao 和 vbo
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // 发送顶点数据到 vbo
    float vertices[108] = {
            -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
            1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
            1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 获取统一变量 transform_mat 的位置
    transform_mat_loc = glGetUniformLocation(program_id, "transform_mat");
    LOG_I("[MultiCubeRender] vao_id: %d, vbo_id: %d, transform_mat_loc: %d, opengl err: %d",
          vao_id, vbo_id, transform_mat_loc, glGetError());
}

MultiCubeRender::~MultiCubeRender() {
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
}

void MultiCubeRender::render() {
    // 清除当前缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 计算透视投影矩阵
    float aspect = 1.0;
    if(height != 0) {
        aspect = width / (float)height;
    }
    glm::mat4 perspective_mat = glm::perspective(1.0742f, aspect, 0.1f, 1000.0f);
    // 计算相机位移的平移矩阵
    glm::mat4 camera_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -40.0f));
    // 逐个渲染元素
    angle += 0.05;
    for(int i = 0; i < CUBES_NUM; ++i) {
         // 随机的平移矩阵
         glm::mat4 cube_translate = glm::translate(glm::mat4(1.0f), glm::vec3(sin(0.35f * i) * 8.0f,
                      cos(0.52f * i) * 7.0f, sin(0.7f * i) * 20.0f));
         // 随机的旋转矩阵
         glm::mat4 cube_rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
         cube_rotation = glm::rotate(cube_rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
         cube_rotation = glm::rotate(cube_rotation, angle, glm::vec3(0.0f, 0.0f, 1.0f));

         // 获取tranform_mat
        glm::mat4 transform_mat = perspective_mat * cube_translate * camera_translation * cube_rotation;

        // 发送数据到 gpu
        glUniformMatrix4fv(transform_mat_loc, 1, GL_FALSE, glm::value_ptr(transform_mat));

        // 设置 vao 属性
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        // 允许深度测试
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        // 进行绘制
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
