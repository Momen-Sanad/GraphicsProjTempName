#include "SceneSerializer.hpp"
#include "../../../vendor/json/single_include/nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using json = nlohmann::json;

string SceneSerializer::lastError;

bool SceneSerializer::saveToFile(const Scene& scene, const string& filePath) {
    try {
        string jsonString = saveToString(scene);
        
        ofstream file(filePath);
        if (!file.is_open()) {
            lastError = "Failed to open file for writing: " + filePath;
            return false;
        }

        file << jsonString;
        file.close();
        return true;
    }
    catch (const exception& e) {
        lastError = "Error saving scene to file: " + string(e.what());
        return false;
    }
}

string SceneSerializer::saveToString(const Scene& scene) {
    json j;

    // Scene metadata
    j["name"] = scene.name;

    // Settings
    json settings;
    settings["clearColor"] = scene.settings.clearColor;
    settings["cameraPosition"] = scene.settings.cameraPosition;
    settings["cameraDirection"] = scene.settings.cameraDirection;
    j["settings"] = settings;

    // Entities
    json entities = json::array();
    for (const auto& entity : scene.entities) {
        json entityJson;
        entityJson["name"] = entity.name;
        
        if (!entity.parent.empty()) {
            entityJson["parent"] = entity.parent;
        }
        
        entityJson["position"] = entity.position;
        entityJson["rotation"] = entity.rotation;
        entityJson["scale"] = entity.scale;
        entityJson["tint"] = entity.tint;
        
        entities.push_back(entityJson);
    }
    j["entities"] = entities;

    return j.dump(2); // Pretty print with 2-space indentation
}

const string& SceneSerializer::getLastError() {
    return lastError;
}