#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // for lookAt and perspective
struct Camera {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 upVec) 
        : position(pos), front(target - pos), up(upVec) {
        updateViewMatrix();
        updateProjectionMatrix(45.0f, 1.0f, 0.1f, 100.0f);
    }

    void updateViewMatrix() {
        viewMatrix = glm::lookAt(position, position + front, up);
    }

    void updateProjectionMatrix(float fov, float aspectRatio, float near, float far) {
        projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
    }
};

#endif // CAMERA_HPP
