#define _USE_MATH_DEFINES
#include "render.h"
#include "scene.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// 核心路径追踪函数
Vec radiance(const Ray &r, int depth, unsigned short *Xi) {
    double t;                         // 相交距离
    int id = 0;                       // 相交物体ID
    
    // 场景相交检测
    if (!scene_intersect(r, t, id)) 
        return Vec(); // 未命中返回黑色
    
    const Sphere &obj = spheres[id];  // 获取相交物体
    Vec x = r.o + r.d * t;            // 交点坐标
    Vec n = (x - obj.p).norm();       // 法线向量
    Vec nl = n.dot(r.d) < 0 ? n : n * -1; // 确保法线方向正确
    Vec f = obj.c;                    // 物体颜色
    
    // 俄罗斯轮盘赌终止条件
    double p = f.x > f.y && f.x > f.z ? f.x : (f.y > f.z ? f.y : f.z);
    if (++depth > 5) {
        if (erand48(Xi) < p) 
            f = f * (1.0 / p);
        else 
            return obj.e; // 提前终止返回发光体颜色
    }

    // 处理不同材质
    if (obj.refl == DIFF) { // 漫反射
        double r1 = 2 * M_PI * erand48(Xi); // 随机角度
        double r2 = erand48(Xi);            // 随机半径
        double r2s = sqrt(r2);
        
        // 创建局部坐标系
        Vec w = nl;
        Vec u = ((fabs(w.x) > 0.1 ? Vec(0, 1) : Vec(1)) % w).norm();
        Vec v = w % u;
        
        // 生成半球随机方向
        Vec d = (u * cos(r1) * r2s + 
                v * sin(r1) * r2s + 
                w * sqrt(1 - r2)).norm();
                
        return obj.e + f.mult(radiance(Ray(x, d), depth, Xi));
    } 
    else if (obj.refl == SPEC) { // 镜面反射
        Vec refl_dir = r.d - n * 2 * n.dot(r.d);
        return obj.e + f.mult(radiance(Ray(x, refl_dir.norm()), depth, Xi));
    } 
    else { // 折射（玻璃材质）
        Ray refl_ray(x, (r.d - n * 2 * n.dot(r.d)).norm());
        bool into = n.dot(nl) > 0;     // 是否从外部进入
        
        // 计算折射率
        double nc = 1.0, nt = 1.5;
        double nnt = into ? nc / nt : nt / nc;
        double ddn = r.d.dot(nl);
        double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
        
        // 全内反射处理
        if (cos2t < 0) 
            return obj.e + f.mult(radiance(refl_ray, depth, Xi));
        
        // 计算折射方向
        Vec tdir = (r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        
        // 菲涅尔效应计算
        double a = nt - nc, b = nt + nc;
        double R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        double Re = R0 + (1 - R0) * c * c * c * c * c;
        double Tr = 1 - Re;
        double P = 0.25 + 0.5 * Re;
        double RP = Re / P, TP = Tr / (1 - P);
        
        // 俄罗斯轮盘赌选择路径
        if (depth > 2) {
            return obj.e + f.mult(erand48(Xi) < P ?
                radiance(refl_ray, depth, Xi) * RP : 
                radiance(Ray(x, tdir), depth, Xi) * TP);
        } else {
            return obj.e + f.mult(
                radiance(refl_ray, depth, Xi) * Re + 
                radiance(Ray(x, tdir), depth, Xi) * Tr);
        }
    }
}

// 渲染主循环
void render_image(Vec* c, int w, int h, int samps) {
    // 相机参数
    Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm());
    Vec cx = Vec(w * 0.5135 / h); // 水平方向向量
    Vec cy = (cx % cam.d).norm() * 0.5135; // 垂直方向向量

    // 多线程渲染提示
    fprintf(stderr, "\nRendering %dx%d (samples=%d)\n", w, h, samps*4);

    // 主渲染循环
    for (int y = 0; y < h; y++) { 
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps*4, 100.*y/(h-1));
        
        // 行渲染循环
        for (unsigned short x = 0, Xi[3] = {0, 0, (unsigned short)(y*y*y)}; x < w; x++) {
            
            // 2x2子采样
            for (int sy = 0; sy < 2; sy++) {     // 上下子像素
                for (int sx = 0; sx < 2; sx++) { // 左右子像素
                    Vec r; // 累计颜色
                    
                    // 采样循环
                    for (int s = 0; s < samps; s++) {
                        
                        // 分层采样（tent filter）
                        double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                        double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                        
                        // 计算光线方向
                        Vec d = cx * (((sx + 0.5 + dx)/2 + x)/w - 0.5) +
                               cy * (((sy + 0.5 + dy)/2 + y)/h - 0.5) + 
                               cam.d;
                        
                        // 累积辐射量
                        r = r + radiance(Ray(cam.o + d*140, d.norm()), 0, Xi) * (1.0/samps);
                    }
                    
                    // 将子像素结果写入缓冲区
                    int index = (h - y - 1) * w + x;
                    c[index] = c[index] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * 0.25;
                }
            }
        }
    }
}