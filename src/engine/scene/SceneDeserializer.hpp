#pragma once

#include "../../../vendor/json/single_include/nlohmann/json.hpp"
#include "Scene.hpp"
#include <string>
#include <memory>

using namespace std;

/**
 * Simple JSON scene loader
 */
class SceneDeserializer {
public:
    static unique_ptr<Scene> loadFromFile(const string& filePath);
    static const string& getLastError();

private:
    static string lastError;
    static vector<float> parseFloatArray(const nlohmann::json& json, size_t expectedSize = 0);
};