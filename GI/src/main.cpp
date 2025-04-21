#include "display.h"
#include "scene.h"
#include "render.h"
#include "camera.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string.h>

//#pragma omp requires unified_shared_memory

// 全局变量声明
Camera camera(Vec(50, 45, 295.6), Vec(0, -0.042612, -1).norm());
Display* display;
bool cameraMoved = false;
double lastTime = 0;

// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = display->w / 2.0, lastY = display->h / 2.0;
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // 反转Y轴方向
    lastX = xpos;
    lastY = ypos;

    camera.process_mouse(xoffset, yoffset);
    cameraMoved = true; // 标记相机移动
}

// 处理键盘输入
void processInput(GLFWwindow* window, double deltaTime) {
    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 处理相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.process_keyboard(GLFW_KEY_W, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.process_keyboard(GLFW_KEY_S, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.process_keyboard(GLFW_KEY_A, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.process_keyboard(GLFW_KEY_D, deltaTime);
        moved = true;
    }

    if (moved) cameraMoved = true;
}

int main() {
    // 初始化显示和场景
    display = new Display(1024, 768);
    init_scene();

    // 设置输入回调
    glfwSetCursorPosCallback(display->window, mouse_callback);
    glfwSetInputMode(display->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int totalSamples = 0; // 累计采样次数
    lastTime = glfwGetTime();

    // 主循环
    while (!glfwWindowShouldClose(display->window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(display->window, deltaTime); // 处理输入

        // 相机移动后重置缓冲区和采样计数
        if (cameraMoved) {
            memset(display->framebuffer, 0, sizeof(Vec) * display->w * display->h);
            totalSamples = 0;
            cameraMoved = false;
        }

        // 渲染图像
        const int SAMPLES_PER_FRAME = 4;
        render_image(display->framebuffer, display->w, display->h, totalSamples, SAMPLES_PER_FRAME, camera);
        
        // 更新纹理并渲染帧
        display->update_texture(totalSamples);
        display->render_frame();

        glfwPollEvents(); // 处理事件
    }

    cleanup_scene(); 
    delete display; // 清理资源
    return 0;
}