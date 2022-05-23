//
// Created by ldh on 2022/4/5.
//

#include "ModelShow.h"
#include "../utils/imported_model.h"
#include <vector>
#include "../utils/jpeg_decode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// 引入模型
static ImportedModel *model = NULL;

ModelShow::ModelShow(AAssetManager *_mgr) {
    // 初始化变量
    vao_id = tex_vbo_id = vert_vbo_id = tex_id = tex_loc = -1;
    rotate_angle = 0.0;
    mgr = _mgr;
    model = NULL;
    // 定义着色器
    vert_shader_src = "#version 300 es     \n"
                      "layout(location = 0) in vec3 position;    \n"
                      "layout(location = 1) in vec2 tex_coord;   \n"
                      "out vec2 tc;                              \n"
                      "uniform mat4 trans_mat;                    \n"
                      "void main()                               \n"
                      "{   \n"
                      "   gl_Position = trans_mat * vec4(position, 1.0);    \n"
                      "   tc = tex_coord;            \n"
                      "}";
    frag_shader_src = "#version 300 es     \n"
                      "layout(binding = 0) uniform sampler2D samp;  \n"
                      "in vec2 tc;   \n"
                      "out vec4 out_color;  \n"
                      "void main()  \n"
                      "{  \n"
                      "   out_color = texture(samp, tc); \n"
                      "}";
    // 加载程序
    load_program();
    LOG_I("[ModelShow] program id: %d, vert shader: %d, frag shader: %d, opengl err: %d",
          program_id, vert_shader, frag_shader, glGetError());
    // 初始化数据
    init_data();
    // 打印
}

ModelShow::~ModelShow() {
    glDeleteTextures(1, &tex_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &tex_vbo_id);
    glDeleteBuffers(1, &vert_vbo_id);
}

Jpeg::Decoder * ModelShow::read_jpeg(std::string jpeg_path) {
    // 打开aaset
    AAsset* asset = AAssetManager_open(mgr, jpeg_path.c_str(), AASSET_MODE_UNKNOWN);
    // 获取长度
    off_t length = AAsset_getLength(asset);
    // 分配内存
    unsigned char *buffer = (unsigned char*) malloc(length + 1);
    // 读取
    AAsset_read(asset, buffer, length);
    // 关闭
    AAsset_close(asset);
    // 新建一个 NV21Image
    Jpeg::Decoder *decoder = new Jpeg::Decoder(buffer, length);
    // 记得释放内存
    free(buffer);
    LOG_I("[ModelShow] Jpeg state: %d, size: (%d, %d)", decoder->GetResult(), decoder->GetWidth(), decoder->GetHeight());
    // 返回结果
    return decoder;
}

void ModelShow::init_data() {
    glUseProgram(program_id);
    // 获取统一变量的位置
    tex_loc = glGetUniformLocation(program_id, "samp");
    trans_mat_loc = glGetUniformLocation(program_id, "trans_mat");
    // 加载模型
    if(model == NULL) {
        model = new ImportedModel(mgr, MODEL_OBJ_NAME);
    }
    LOG_I("[ModelShow] Model vertices num: %d", model->get_vetices_num());
    // 加载数据
    auto verts_model = model->get_vertices();
    auto tex_model = model->get_texture_coords();
    vert_num = model->get_vetices_num();
    vector<float> vertices;
    vector<float> tex_coord;
    vertices.reserve(vert_num * 3);
    tex_coord.reserve(vert_num * 2);
    for(int i = 0; i < vert_num; ++i) {
        vertices.push_back(verts_model[i].x);
        vertices.push_back(verts_model[i].y);
        vertices.push_back(verts_model[i].z);

        tex_coord.push_back(tex_model[i].s);
        tex_coord.push_back(tex_model[i].t);
    }
    // 生成vao
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    // 生成vbo并发送数据
    glGenBuffers(1, &vert_vbo_id);
    glGenBuffers(1, &tex_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tex_coord.size(), tex_coord.data(), GL_STATIC_DRAW);
    // 生成纹理并发送数据
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    Jpeg::Decoder *decoer = read_jpeg(MODEL_IMAGE_NAME);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, decoer->GetWidth(), decoer->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, decoer->GetImage());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LOG_I("[ModelShow] vao_id: %d, vbo_id: %d %d, tex_id: %d, tex_loc: %d, trans_mat_loc: %d",
         vao_id, vert_vbo_id, tex_vbo_id, tex_id, tex_loc, trans_mat_loc, glGetError());
    // 释放资源
    delete decoer;
}

void ModelShow::render() {
    // 清理缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 进行旋转, 绕着y轴转
    rotate_angle += 0.01;
    glm::mat4 pers = glm::perspective(1.0472f, (float)width / height, 0.1f, 1000.0f);  // 透视投影
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, -0.5f));    // 平移
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotate_angle, glm::vec3(0.0f, 1.0f, 0.0f));  //旋转
    trans = pers * trans * rot;
    glUniformMatrix4fv(trans_mat_loc, 1, GL_FALSE, glm::value_ptr(trans));
    // 设置顶点属性
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    // 设置纹理属性
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glUniform1i(tex_loc, 0);
    // 允许深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, vert_num);
}

