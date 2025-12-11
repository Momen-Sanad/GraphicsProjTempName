#pragma once

#include <string>
#include <vector>

using namespace std;

/**
 * Simple scene data structure for JSON loading
 */
class Scene {
public:
    struct EntityData {
        string name;
        string parent = "";                     // parent entity name (empty = root)
        vector<float> position = {0.0f, 0.0f, 0.0f};
        vector<float> rotation = {0.0f, 0.0f, 0.0f};  // euler angles in degrees
        vector<float> scale = {1.0f, 1.0f, 1.0f};
        vector<float> tint = {1.0f, 1.0f, 1.0f, 1.0f};  // tint color for materials
    };

    struct SceneSettings {
        vector<float> clearColor = {0.2f, 0.3f, 0.4f, 1.0f};
        vector<float> cameraPosition = {0.0f, 0.0f, 5.0f};
        vector<float> cameraDirection = {0.0f, 0.0f, -1.0f};
    };

public:
    string name;
    vector<EntityData> entities;
    SceneSettings settings;
};