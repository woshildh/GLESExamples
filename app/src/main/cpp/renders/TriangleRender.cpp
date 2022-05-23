//
// Created by ldh on 2022/3/29.
//

#include "TriangleRender.h"
#include <GLES3/gl3.h>

TriangleRender::TriangleRender() {
    vert_shader_src = "#version 300 es                          \n"
                      "layout(location = 0) in vec4 position;   \n"
                      "out vec4 color;                          \n"
                      "void main()                              \n"
                      "{                                         \n"
                      "    gl_Position = position;              \n"
                      "    color = position;                    \n"
                      "}";
    frag_shader_src = "#version 300 es                          \n"
                      "out vec4 outColor;                       \n"
                      "in vec4 color;                         \n"
                      "void main()                        \n"
                      "{                                        \n"
                      "    outColor = vec4(color.x + 0.2, color.y + 0.2, color.z + 0.2, 1.0);    \n"
                      "}";
    program_id = vert_shader = frag_shader = 0;
    // 加载程序
    load_program();


}

TriangleRender::~TriangleRender() {
}

void TriangleRender::render() {
    // 设置clearColor
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // 设置使用的程序
    glUseProgram(program_id);
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);
    // 定义三个点的坐标
    float vertices[] = {
            0.0, 0.0, 0.0,
            0.5, 0.5, 0.0,
            0.5, -0.5, 0.0
    };
    LOG_I("program id: %d, width: %d, height: %d", program_id, width, height);

    // 设置视角
    glViewport(0, 0, width, height);
    // 发送数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
