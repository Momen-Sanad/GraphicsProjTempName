#pragma once

#include <string>
#include <vector>

/**
 * Simple scene data structure for JSON loading
 */
class Scene {
public:
    struct EntityData {
        std::string name;
        std::string parent = "";                     // parent entity name (empty = root)
        std::vector<float> position = {0.0f, 0.0f, 0.0f};
        std::vector<float> rotation = {0.0f, 0.0f, 0.0f};  // euler angles in degrees
        std::vector<float> scale = {1.0f, 1.0f, 1.0f};
        std::vector<float> tint = {1.0f, 1.0f, 1.0f, 1.0f};  // tint color for materials
    };

    struct SceneSettings {
        std::vector<float> clearColor = {0.2f, 0.3f, 0.4f, 1.0f};
        std::vector<float> cameraPosition = {0.0f, 0.0f, 5.0f};
        std::vector<float> cameraDirection = {0.0f, 0.0f, -1.0f};
    };

public:
    std::string name;
    std::vector<EntityData> entities;
    SceneSettings settings;

    // Utility methods
    EntityData* findEntity(const std::string& name);
    void addEntity(const EntityData& entity);
    void removeEntity(const std::string& name);
    bool hasEntity(const std::string& name) const;
    size_t getEntityCount() const;
    void clear();
};