#include "camera.h"
#include <cmath>

Camera::Camera(Vec pos, Vec dir) : 
    position(pos), front(dir.norm()), up(0,1,0), 
    yaw(-M_PI/2), pitch(0), moveSpeed(7), mouseSensitivity(0.001) {
    update_vectors();
}

void Camera::update_vectors() {
    Vec newFront;
    newFront.x = cos(yaw) * cos(pitch);
    newFront.y = sin(pitch);
    newFront.z = sin(yaw) * cos(pitch);
    front = newFront.norm();
    right = (front % up).norm();
}

void Camera::process_keyboard(int key, double deltaTime) {
    double velocity = moveSpeed * deltaTime;
    if (key == GLFW_KEY_W) position = position + front * velocity;
    if (key == GLFW_KEY_S) position = position - front * velocity;
    if (key == GLFW_KEY_A) position = position - right * velocity;
    if (key == GLFW_KEY_D) position = position + right * velocity;
}

void Camera::process_mouse(double xoffset, double yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw   += xoffset;
    pitch += yoffset;
    pitch = std::max(std::min(pitch, M_PI/2 - 0.1), -M_PI/2 + 0.1);
    update_vectors();
}