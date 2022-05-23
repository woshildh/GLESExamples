//
// Created by ldh on 2022/4/10.
//

#include "SnowRender.h"
#include <glm/gtc/type_ptr.hpp>

SnowRender::SnowRender(AAssetManager *_mgr, int num) : snow_num(num), data(snow_num), mgr(_mgr) {
    // 初始化变量
    vert_shader_src = "#version 300 es   \n"
                      "uniform mat4 transform_mat;  // 变换矩阵 \n"
                      "layout(location = 0) in vec4 vert_position;    \n"
                      "void main()    \n"
                      "{   \n"
                      "    gl_Position = transform_mat * vert_position;  \n"
                      "}";
    frag_shader_src = "#version 300 es   \n"
                      "out vec4 out_color; \n"
                      "void main()    \n"
                      "{  \n"
                      "    out_color = vec4(0.9, 0.9, 0.9, 0.9); \n"
                      "}";
    // 加载当前程序
    load_program();
    LOG_I("[SnowRender] program id: %d", program_id);
    // 初始化数据据
    init_vertices_data();
}

SnowRender::~SnowRender() {
    if(model) {
        delete model;
        model = nullptr;
    }
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
}

void SnowRender::init_vertices_data() {
    glUseProgram(program_id);
    // 导入模型
    model = new ImportedModel(mgr, "snow.obj");
    // 生成vao和vbo
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // 发送顶点数据到 vbo
    std::vector<glm::vec3> pts = model->get_vertices();
    snow_pts_num = model->get_vetices_num();
    size_t sz = model->get_vetices_num();
    std::vector<float> vertices(sz);
    for(int i = 0; i < sz; ++i) {
        vertices[i * 3] = pts[i].x;
        vertices[i * 3 + 1] = pts[i].y;
        vertices[i * 3 + 2] = pts[i].z;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // 统一变量的位置
    trans_mat_loc = glGetUniformLocation(program_id, "transform_mat");
    LOG_I("[SnowRender] vao_id: %d, vbo_id: %d, trans_mat_loc: %d, state: %d", vao_id, vbo_id, trans_mat_loc, glGetError());
}

void SnowRender::render() {
    // 清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置 vao 属性
    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    // 设置透视矩阵
    glm::mat4 pers_mat = glm::perspective(1.0472f, width / (height + 0.001f), 0.1f, 100.0f);
    init_pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    for(int i = 0; i < snow_num; ++i) {
        // 获取所有矩阵
        glm::mat4 r_mat = data[i].get_rotate_mat();
        glm::mat4 t_mat = data[i].get_trans_mat();
        glm::mat4 s_mat = data[i].get_scale_mat();

        // 进行变换
        glm::mat4 transform_mat = pers_mat * init_pos * t_mat * s_mat * r_mat;
        glUniformMatrix4fv(trans_mat_loc, 1, GL_FALSE, glm::value_ptr(transform_mat));
        // 打开深度测试
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        // 进行绘制
        glDrawArrays(GL_TRIANGLES, 0, snow_pts_num);
        // 更新当前数据
        data[i].update_data();
    }
}


