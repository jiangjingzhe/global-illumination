#pragma once
#include "geometry.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Camera {
public:
    Vec position, front, up, right; // 相机位置、前向、上向、右向向量
    double yaw, pitch; 
    double moveSpeed;
    double mouseSensitivity;

    Camera(Vec pos = Vec(50, 52, 295.6), Vec dir = Vec(0, -0.042612, -1));//Vec pos = Vec(50, 52, 295.6)
    void update_vectors();
    void process_keyboard(int key, double deltaTime);
    void process_mouse(double xoffset, double yoffset);
};