#include "scene.h"
#include <vector>

Sphere* spheres = nullptr; // 动态初始化
int num_spheres = 0;

void init_scene() {
    std::vector<Sphere> scene_spheres = {
        Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//左墙面
        Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//右墙面
        Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//后墙面
        Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//前墙面
        Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//底面
        Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//顶面
        Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//镜面反射
        Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//玻璃球
        Sphere(600, Vec(50,681.6-.27,81.6),Vec(120,120,120),  Vec(.75,.25,.25), DIFF) //发光体
    };
    num_spheres = scene_spheres.size();
    spheres = new Sphere[num_spheres];
    std::copy(scene_spheres.begin(), scene_spheres.end(), spheres);
}

bool scene_intersect(const Ray &r, double &t, int &id) {
    const float epsilon = 1e-4f;
    float inf = t = 1e20f;
    int tempId = -1;
    
    #pragma omp simd
    for (int i = 0; i < num_spheres; i++) {
        const float d = spheres[i].intersect(r);
        if (d > epsilon && d < t) {
            t = d;
            tempId = i;
        }
    }
    
    id = tempId;
    return (tempId != -1);
}

void cleanup_scene() {
    delete[] spheres;
    spheres = nullptr;
    num_spheres = 0;
}