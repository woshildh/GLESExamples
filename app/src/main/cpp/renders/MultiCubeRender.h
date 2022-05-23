//
// Created by ldh on 2022/4/1.
//

#ifndef GLESEXAMPLES_MULTICUBERENDER_H
#define GLESEXAMPLES_MULTICUBERENDER_H

#include "BaseRender.h"

#define CUBES_NUM 30

class MultiCubeRender : public BaseRender {
public:
    MultiCubeRender();
    virtual ~MultiCubeRender();
    virtual void render() override;
private:
    // 初始化顶点数据
    void init_vertices();
    // vao 的 id
    GLuint vao_id;
    // vbo 的 id
    GLuint vbo_id;
    // 变换矩阵
    GLuint transform_mat_loc;
    // 初始角度
    float angle;
};


#endif //GLESEXAMPLES_MULTICUBERENDER_H
