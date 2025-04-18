#include "geometry.h"
#include "scene.h"
#include "render.h"
#include "utils.h"
#include "display.h"
#include "camera.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Camera camera;
Display* display;
bool needReset = true;
double lastTime = 0;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = display->w/2, lastY = display->h/2;
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.process_mouse(xoffset, yoffset);
    needReset = true;
}

int main() {
    display = new Display(512, 384);
    init_scene();

    glfwSetCursorPosCallback(display->window, mouse_callback);
    glfwSetInputMode(display->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(display->window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 处理输入
        if (glfwGetKey(display->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;
        if (glfwGetKey(display->window, GLFW_KEY_W) == GLFW_PRESS)
            camera.process_keyboard(GLFW_KEY_W, deltaTime);
        if (glfwGetKey(display->window, GLFW_KEY_S) == GLFW_PRESS)
            camera.process_keyboard(GLFW_KEY_S, deltaTime);
        if (glfwGetKey(display->window, GLFW_KEY_A) == GLFW_PRESS)
            camera.process_keyboard(GLFW_KEY_A, deltaTime);
        if (glfwGetKey(display->window, GLFW_KEY_D) == GLFW_PRESS)
            camera.process_keyboard(GLFW_KEY_D, deltaTime);

        // 重置累积缓冲区
        if(needReset) {
            memset(display->framebuffer, 0, sizeof(Vec)*display->w*display->h);
            needReset = false;
        }

        // 渲染并显示
        render_image(display->framebuffer, display->w, display->h, 100, camera);
        display->update_texture();
        display->render_frame();
        glfwPollEvents();
    }

    delete display;
    return 0;
}