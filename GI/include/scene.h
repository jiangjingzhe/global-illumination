#pragma once
#include "geometry.h"

extern Sphere* spheres;     // 场景物体数组
extern int num_spheres;     // 物体数量
void init_scene();        // 初始化场景函数
bool scene_intersect(const Ray &r, double &t, int &id); // 场景级碰撞检测
void cleanup_scene();     // 清理场景函数