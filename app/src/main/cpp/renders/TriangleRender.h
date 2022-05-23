//
// Created by ldh on 2022/3/29.
//

#ifndef GLESEXAMPLES_TRIANGLERENDER_H
#define GLESEXAMPLES_TRIANGLERENDER_H

#include "BaseRender.h"

// 这个是渲染三角形的渲染器
class TriangleRender : public BaseRender  {
public:
    TriangleRender();
    virtual ~TriangleRender();
    virtual void render() override;
};


#endif //GLESEXAMPLES_TRIANGLERENDER_H
