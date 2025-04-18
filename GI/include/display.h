#pragma once
#include "geometry.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Display {
private:
    GLuint quadVAO, quadVBO; // 全屏四边形渲染所需
    GLuint shaderProgram;    // 着色器程序
    
public:
    GLFWwindow* window;
    GLuint renderTexture;
    int w, h;
    Vec* framebuffer;

    Display(int width, int height);
    ~Display();
    void init_opengl();
    void setup_quad(); // 设置全屏四边形
    void update_texture();
    void render_frame();
    
private:
    void compile_shaders(); // 着色器编译
};