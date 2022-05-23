//
// Created by ldh on 2022/4/2.
//

#ifndef GLESEXAMPLES_CUBETEXTURERENDER_H
#define GLESEXAMPLES_CUBETEXTURERENDER_H

#include "BaseRender.h"
#include <android/asset_manager_jni.h>

class CubeTextureRender : public BaseRender {

public:
    CubeTextureRender(AAssetManager *_mgr);
    virtual ~CubeTextureRender();
    virtual void render() override;

private:
    // 初始化所有数据
    void init_data();
    // 用于资源管理
    AAssetManager *mgr;
    // 用于管理vao 和 vbo
    GLuint vao_id;
    GLuint vbo_position_id;
    GLuint vbo_texture_id;
    // 管理 texture
    GLuint texture_id;
    // transform_mat 位置
    GLint trans_mat_loc;
    // 旋转角度
    float angle;
};


#endif //GLESEXAMPLES_CUBETEXTURERENDER_H
