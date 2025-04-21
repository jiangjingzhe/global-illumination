#define _USE_MATH_DEFINES
#include "render.h"
#include "scene.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


// 核心路径追踪函数
Vec radiance(const Ray &r, int depth, unsigned short *Xi) {
    double t;                         // 相交距离
    int id = 0;                       // 相交物体ID

    if (depth < 0) {
        return Vec(); // 返回黑色防止崩溃
    }

    // 场景相交检测
    if (!scene_intersect(r, t, id)) 
        return Vec(); // 未命中返回黑色
    
    const Sphere &obj = spheres[id];  // 获取相交物体
    Vec x = r.o + r.d * t;            // 交点坐标
    Vec n = (x - obj.p).norm();       // 法线向量
    Vec nl = n.dot(r.d) < 0 ? n : n * -1; // 确保法线方向正确
    Vec f = obj.c;                    // 物体颜色
    
    // 自发光贡献
    Vec emitted = (obj.e.x > 0 || obj.e.y > 0 || obj.e.z > 0) ? obj.e : Vec();
    // if(emitted.x > 0 || emitted.y > 0 || emitted.z > 0) 
    //     printf("Emitted color: (%f, %f, %f)\n", emitted.x, emitted.y, emitted.z);

    if (depth > 30) return emitted;
    
    // 俄罗斯轮盘赌终止条件
    if (++depth > 8) {  
        double p = f.x > f.y && f.x > f.z ? f.x : (f.y > f.z ? f.y : f.z);
        p = std::max(p, 0.1); // 避免过小的概率值
        if (erand48(Xi) >= p) 
            return emitted;  // 提前终止返回发光
        f = f*(1.0/p);       // 补偿能量
    }
    
    // 直接光源采样
    if (obj.refl == DIFF) {
        // 遍历所有光源
        Vec directLight = Vec();
        for (int i = 0; i < num_spheres; ++i) {
            if (!(spheres[i].e.x > 0 || spheres[i].e.y > 0 || spheres[i].e.z > 0)) 
                continue; // 跳过非光源
            
            const Sphere &light = spheres[i];
            Vec lightDir = light.p - x;
            double lightDist2 = lightDir.dot(lightDir);
            lightDir = lightDir.norm();
            
            // 构建阴影射线
            Ray shadowRay(x + nl*1e-3, lightDir);
            double lightDist = sqrt(lightDist2);
            double t_light;
            int id_light;
            
            // 阴影检测
            if (!scene_intersect(shadowRay, t_light, id_light) || (t_light*t_light > lightDist2)) {
                // 正确计算立体角（光源表面积投影）
                double cosThetaLight = sqrt(1.0 - (light.rad*light.rad)/lightDist2);
                if (lightDist2 < light.rad*light.rad) cosThetaLight = 0; 
                double omega = 2 * M_PI * (1 - cosThetaLight);
                double cosTheta = nl.dot(lightDir);
                if (cosTheta > 0) {
                    Vec brdf = f * (1.0/M_PI);
                    directLight = directLight + brdf.mult(light.e) * cosTheta * omega;
                }
            }
        }
        emitted = emitted + directLight;
    }
    
    // 材质处理
    switch (obj.refl) {
        case DIFF: { // 漫反射
            double r1 = 2*M_PI*erand48(Xi);
            double r2 = erand48(Xi);
            double r2s = sqrt(r2);
            
            // 构建局部坐标系
            Vec w = nl;
            Vec u = ((fabs(w.x) > 0.1 ? Vec(0,1) : Vec(1))%w).norm();
            Vec v = w%u;
            
            // 半球采样
            Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1 - r2)).norm();
            return emitted + f.mult(radiance(Ray(x, d), depth, Xi));
        }
        case SPEC: { // 镜面反射
            Vec reflDir = r.d - n*2*n.dot(r.d);
            return emitted + f.mult(radiance(Ray(x, reflDir.norm()), depth, Xi));
        }
        case REFR: { // 折射
            Ray reflRay(x, (r.d - n*2*n.dot(r.d)).norm());
            bool into = n.dot(nl) > 0;
            double nc = 1.0, nt = 1.5;
            double nnt = into ? nc/nt : nt/nc;
            double ddn = r.d.dot(nl);
            double cos2t = 1 - nnt*nnt*(1 - ddn*ddn);
            
            // 全反射处理
            if (cos2t < 0) 
                return emitted + f.mult(radiance(reflRay, depth, Xi));
            
            Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt + sqrt(cos2t)))).norm();
            double a = nt - nc, b = nt + nc;
            double R0 = a*a/(b*b), c = 1 - (into ? -ddn : tdir.dot(n));
            double Re = R0 + (1 - R0)*c*c*c*c*c;
            double Tr = 1 - Re;
            
            // 重要性采样
            if (depth > 2) {
                double P = 0.25 + 0.5*Re;
                if (erand48(Xi) < P)
                    return emitted + f.mult(radiance(reflRay, depth, Xi)*(Re/P));
                else
                    return emitted + f.mult(radiance(Ray(x, tdir), depth, Xi)*(Tr/(1 - P)));
             }
            return emitted + f.mult(
                radiance(reflRay, depth, Xi)*Re + 
                radiance(Ray(x, tdir), depth, Xi)*Tr);
         }
        default:
            return emitted;
    }
}


// 渲染函数
void render_image(Vec* c, int w, int h, int &totalSamples, int addSamples, const Camera& cam) {
    Vec cx = Vec(w * 0.5135 / h , 0 , 0 );
    Vec cy = (cx % cam.front).norm() * 0.5135 ;
    //Vec cy = (cx % cam.front).norm() * 0.5135;
    Vec camPos = cam.position;

    printf("Rendering %d samples...\n", totalSamples + addSamples);

    //主渲染循环
    #pragma omp parallel for schedule(dynamic, 4) collapse(2)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x){
            unsigned short Xi[3] = { 
                static_cast<unsigned short>((x << 8) | y),
                static_cast<unsigned short>(totalSamples),
                static_cast<unsigned short>(omp_get_thread_num())  // 增加线程标识
            };
            
            Vec pixelColor = (totalSamples > 0) ? c[y*w+x] : Vec();
            
            // 增量采样循环
            
            // 生成抗锯齿采样坐标
            const double r1 = 2 * erand48(Xi);
            const double r2 = 2 * erand48(Xi);
            const double dx = (r1 < 1) ? sqrt(r1)-1 : 1-sqrt(2-r1);
            const double dy = (r2 < 1) ? sqrt(r2)-1 : 1-sqrt(2-r2);
                
            // 计算光线方向
            Vec rayDir = (cx * ((x + dx/2)/w - 0.5) + cy * ((y + dy/2)/h - 0.5) + cam.front).norm();
                
            // 路径追踪计算
            Vec sample = radiance(Ray(camPos + rayDir*140, rayDir.norm()), 0, Xi);
                
            // 增量累积（Welford算法）
            if (totalSamples == 0 )
                pixelColor = sample;
            else {                    
                const double n = totalSamples + 1;
                pixelColor = (pixelColor * (n-1) + sample) / n;
            }
            
            c[y*w+x] = pixelColor;
            //printf("\rPixel (\t%d, \t%d): (%f, %f, %f)", x, y, pixelColor.x, pixelColor.y, pixelColor.z);
        }
    }
    
    // 更新总采样数
    #pragma omp atomic
    totalSamples += addSamples;
}