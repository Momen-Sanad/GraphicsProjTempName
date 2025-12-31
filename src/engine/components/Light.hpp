#pragma once
#include <glm/glm.hpp>

enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

struct Light {

    LightType type;
    glm::vec3 color;
    glm::vec3 position;  // Only for point and spot lights
    glm::vec3 direction; // Only for directional and spot lights
    float innerAngle;    // For spot lights
    float outerAngle;    // For spot lights
    float intensity;     // Light intensity

    Light(LightType type, 
          glm::vec3 color, 
          glm::vec3 position = glm::vec3(0.0f), 
          glm::vec3 direction = glm::vec3(0.0f),
          float innerAngle = glm::radians(15.0f), 
          float outerAngle = glm::radians(30.0f), 
          float intensity = 1.0f)
        : type(type), 
          color(color), 
          position(position), 
          direction(direction), 
          innerAngle(innerAngle), 
          outerAngle(outerAngle), 
          intensity(intensity) {}
};
