#pragma once
#include "geometry.h"
#include "camera.h"

Vec radiance(const Ray &r, int depth, unsigned short *Xi); // 路径追踪核心
void render_image(Vec* c, int w, int h, int &totalSamples, int addSamples, const Camera& cam);        // 渲染循环控制