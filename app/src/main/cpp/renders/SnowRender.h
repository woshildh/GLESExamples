//
// Created by ldh on 2022/4/10.
//

#ifndef GLESEXAMPLES_SNOWRENDER_H
#define GLESEXAMPLES_SNOWRENDER_H

#include "BaseRender.h"
#include <glm/glm.hpp>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "../utils/imported_model.h"

#define LDH_PI 3.1416

struct SnowData {
private:
    float pos_x;   // x 方向的坐标
    float pos_y;   // y 方向的坐标
    float step_y;   // y 方向的步长，以帧为时间单位
    float rotate_y;   // 以 y 轴为中心轴的旋转
    float scale;   // 缩放系数
    void init_params() {
        pos_x = (rand() % 100) * 0.04 + -2;
        pos_y = 2.0 + rand() % 20 * 0.05;
        step_y = (rand() % 3 + 2) * 0.002;
        rotate_y = (rand() % 360) / 360.0 * 2 * LDH_PI;
        scale = (rand() % 5) * 0.15;
    }
public:
    SnowData() {
        init_params();
    }
    ~SnowData() {

    }
    // 获取旋转矩阵
    glm::mat4 get_rotate_mat() {
        glm::mat4 r_mat = glm::rotate(glm::mat4(1.0), rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));
        return r_mat;
    }
    // 获取平移矩阵
    glm::mat4 get_trans_mat() {
        glm::mat4 t_mat = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x, pos_y, 0.0f));
        return t_mat;
    }
    // 获取缩放矩阵
    glm::mat4 get_scale_mat() {
        glm::mat4 s_mat = glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale));
        return s_mat;
    }
    // 更新当前信息
    void update_data() {
        if(pos_y < -2.0) {
            init_params();
        } else {
            pos_y -= step_y;
        }
    }
};

class SnowRender : public BaseRender {
public:
    SnowRender(AAssetManager *_mgr, int num = 1000);
    virtual ~SnowRender();
    virtual void render() override;
private:
    // android 管理对象
    AAssetManager *mgr = nullptr;
    // 雪花数量
    int snow_num = 100;
    // 雪花中的顶点数据量
    int snow_pts_num = 0;
    // 存放所有的雪花数据
    std::vector<SnowData> data;
    // 导入的雪花3D模型
    ImportedModel *model = nullptr;
    // 初始化位置
    glm::mat4 init_pos;
    // OpenGl 相关的数据模型
    GLuint vao_id;
    GLuint vbo_id;
    GLint trans_mat_loc;  // 变换矩阵统一变量位置
    // 初始化顶点数据
    void init_vertices_data();
};


#endif //GLESEXAMPLES_SNOWRENDER_H
