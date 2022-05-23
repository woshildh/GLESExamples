//
// Created by ldh on 2022/3/30.
//

#ifndef GLESEXAMPLES_CUBERENDER_H
#define GLESEXAMPLES_CUBERENDER_H

#include "BaseRender.h"
#include "glm/glm.hpp"
#include <GLES3/gl3.h>

class CubeRender : public BaseRender {
public:
    CubeRender();
    virtual ~CubeRender();
    virtual void render() override;

private:
    // 旋转矩阵
    glm::mat4 rotation_mat;
    // X,Y,Z 轴的旋转角度 单位是弧度
    float rotate_x;
    float rotate_y;
    float rotate_z;
    // rotation 统一变量在 opengl 程序中的位置
    GLuint rotate_loc;
    // vao 和 vbo 的id
    GLuint vao_id;
    GLuint vbo_id;

    // 初始化顶点数据
    void init_vertices_data();
};


#endif //GLESEXAMPLES_CUBERENDER_H
