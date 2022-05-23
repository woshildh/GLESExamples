//
// Created by ldh on 2022/3/30.
//

#include "CubeRender.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

CubeRender::CubeRender() {
    // 设置代码
    vert_shader_src = "#version 300 es    \n"
                      "layout(location = 1) in vec4 position;  \n"
                      "uniform mat4 rotation;       \n"
                      "void main() \n"
                      "{   \n"
                      "     gl_Position = rotation * position;  \n"
                      "}\n";

    frag_shader_src = "#version 300 es  \n"
                      "out vec4 outColor;  \n"
                      "void main()  \n"
                      "{  \n"
                      "    outColor = vec4(1.0, 0.0, 0.0, 1.0);  \n"
                      "}\n";
    // 初始化
    program_id = vert_shader = frag_shader = vao_id = vbo_id = 0;
    rotate_x = rotate_y = rotate_z = 0;
    // 加载程序
    load_program();
    LOG_I("[CubeRender] render state: %d", glGetError());
    // 初始化顶点数据
    init_vertices_data();
    // 打日志
    LOG_I("[CubeRender debug] program id: %d, vao: %d, vbo: %d, rotate_loc: %d, err: %d", program_id, vao_id, vbo_id,
          rotate_loc, glGetError());
}

CubeRender::~CubeRender() noexcept {
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
}

void CubeRender::init_vertices_data() {
    // 顶点坐标
    float vertexPositions[108] = {
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
    // 生成 vao
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    // 生成 vbo
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // 发送顶点数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    // 获取统一变量的位置
    rotate_loc = glGetUniformLocation(program_id, "rotation");
    // 指定vao属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}

void CubeRender::render() {
    // 使用当前程序
    glUseProgram(program_id);
    // 设置清除色
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 求解投影和偏移矩阵
    glm::mat4 proj_mat = glm::perspective(1.0472f, width / (height + 0.01f), 0.1f, 1000.0f);
    glm::mat4 trans_mat1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -10.0f));
    // 旋转矩阵
    rotate_x += 0.02; rotate_y += 0.04; rotate_z += 0.01;
    rotation_mat = glm::rotate(glm::mat4(1.0f), rotate_x, glm::vec3(1.0, 0.0, 0.0));
    rotation_mat = glm::rotate(rotation_mat, rotate_y, glm::vec3(0.0, 1.0, 0.0));
    rotation_mat = glm::rotate(rotation_mat, rotate_z, glm::vec3(0.0, 0.0, 1.0));
    // 将所有变换进行组合
    rotation_mat = proj_mat * trans_mat1 * rotation_mat;
    // 发送统一变量数据数据
    glUniformMatrix4fv(rotate_loc, 1, GL_FALSE, glm::value_ptr(rotation_mat));
    // 打开深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //
    LOG_I("[CubeRender] state: %d", glGetError());
}