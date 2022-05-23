//
// Created by ldh on 2022/4/4.
//

#ifndef GLESEXAMPLES_IMAGECHANGERENDER_H
#define GLESEXAMPLES_IMAGECHANGERENDER_H

#include "BaseRender.h"
#include "../utils/jpeg_decode.h"
#include <string>
#include <android/asset_manager_jni.h>

#define IMAGE1_NAME "liudehua1.jpg"
#define IMAGE2_NAME "liudehua2.jpg"

class ImageChangeRender : public BaseRender {
public:
    ImageChangeRender(AAssetManager *_mgr);
    virtual ~ImageChangeRender();
    virtual void render() override;

private:
    // 读取assets资源的管理类
    AAssetManager *mgr;
    // 读取图像
    Jpeg::Decoder *read_jpeg(std::string img_path);
    // 加载数据
    void init_data();
    // 两张图像的 teture_id 以及 统一变量位置
    GLuint texture_id[2];
    GLuint texture_uni_loc[2];
    //vao 和 vbo
    GLuint vao_id;
    GLuint vbo_id;
    // 两张图像的交叉高度
    float cross_height;
    int positive;
    GLuint cross_height_loc;
};


#endif //GLESEXAMPLES_IMAGECHANGERENDER_H
