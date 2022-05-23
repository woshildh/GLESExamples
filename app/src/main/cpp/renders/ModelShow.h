//
// Created by ldh on 2022/4/5.
//

#ifndef GLESEXAMPLES_MODELSHOW_H
#define GLESEXAMPLES_MODELSHOW_H

#include "BaseRender.h"
#include <android/asset_manager_jni.h>
#include "../utils/imported_model.h"
#include "../utils/jpeg_decode.h"

//#define MODEL_OBJ_NAME "shuttle.obj"
//#define MODEL_IMAGE_NAME "shuttle.jpg"

#define MODEL_OBJ_NAME "red_dragon.obj"
#define MODEL_IMAGE_NAME "red_dragon.jpeg"



class ModelShow : public BaseRender{
public:
    ModelShow(AAssetManager *mgr);
    virtual ~ModelShow();
    virtual void render();

private:
    // 初始化数据, 还需要加载模型
    void init_data();
    Jpeg::Decoder *read_jpeg(std::string jpeg_path);
    // 本地文件读取管理器
    AAssetManager *mgr;
    // 顶点
    size_t vert_num;
    GLuint vao_id;
    GLuint vert_vbo_id;
    GLuint tex_vbo_id;
    // 纹理
    GLuint tex_id;
    GLint tex_loc;
    // 旋转相关
    float rotate_angle;
    GLint trans_mat_loc;
};


#endif //GLESEXAMPLES_MODELSHOW_H
