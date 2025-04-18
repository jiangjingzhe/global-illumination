#pragma once
#include "geometry.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Camera {
public:
    Vec position, front, up, right;
    double yaw, pitch;
    double moveSpeed;
    double mouseSensitivity;

    Camera(Vec pos = Vec(50, 52, 295.6), Vec dir = Vec(0, -0.042612, -1));
    void update_vectors();
    void process_keyboard(int key, double deltaTime);
    void process_mouse(double xoffset, double yoffset);
};