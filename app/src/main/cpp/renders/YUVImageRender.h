//
// Created by ldh on 2022/4/4.
//

#ifndef GLESEXAMPLES_YUVIMAGERENDER_H
#define GLESEXAMPLES_YUVIMAGERENDER_H

#include "BaseRender.h"
#include <android/asset_manager_jni.h>

#define YUV_IMAGE_WIDTH 840
#define YUV_IMAGE_HEIGHT 1074
#define YUV_FILE_NAME "YUV_Image_840x1074.NV21"

// 只支持 Nv21 的图像格式
struct NV21Image {
public:
    NV21Image(unsigned char* buf, int _width, int _height) {
        width = _width, height = _height;
        // 分配内存
        y_plane = (unsigned char*)malloc(width * height);
        u_plane = (unsigned char*)malloc(width * height / 2 + 5);
        v_plane = (unsigned char*)malloc(width * height / 2 + 5);
        // 拷贝 y 平面
        memcpy(y_plane, buf, width * height);
        // 逐个像素进行拷贝
        size_t off = width * height;
        for(int i = 0; i < width * height / 4; ++i) {
            u_plane[i] = buf[off + i * 2 + 1];
            v_plane[i] = buf[off + i * 2];
        }
    }
    ~NV21Image() {
        if(y_plane) free(y_plane);
        if(u_plane) free(u_plane);
        if(v_plane) free(v_plane);
    }
    int width;
    int height;
    unsigned char* y_plane;
    unsigned char* u_plane;
    unsigned char* v_plane;
};

class YUVImageRender : public BaseRender {
public:
    YUVImageRender(AAssetManager *_mgr, int _w, int _h);
    virtual ~YUVImageRender();
    virtual void render() override;
private:
    // 读取yuv数据
    NV21Image* read_yuv();
    // 初始化数据
    void init_data();
    // 计算顶点位置坐标
    void calculate_vertices(float vertices[16]);
    // 管理本地资源
    AAssetManager *mgr;
    // 纹理 id
    GLuint texture_id[3];
    GLint texture_loc[3];
    // vao id
    GLuint vao_id;
    // vbo id
    GLuint vbo_id;
    // trans_mat 的位置
    GLint trans_mat_loc;
    // z轴方向的偏移
    float trans_z;
    int positive;
};


#endif //GLESEXAMPLES_YUVIMAGERENDER_H
