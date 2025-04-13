#include "geometry.h"
#include "scene.h"
#include "render.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    init_scene(); // 初始化场景
    
    const int w = 1024/8, h = 768/8;
    int samps = argc >=2 ? atoi(argv[1])/4 : 30;
    Vec* c = new Vec[w*h];
    
    render_image(c, w, h, samps);
    
    // 输出PPM文件
    FILE *f = fopen("image.ppm", "w");
    fprintf(f, "P3\n%d %d\n255\n", w, h);
    for (int i=0; i<w*h; i++)
        fprintf(f,"%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
    delete[] c;
    return 0;
}