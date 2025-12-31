#pragma once

#include "Scene.hpp"
#include <string>

using namespace std;

/*
 * Simple JSON scene writer
 */
class SceneSerializer {
private:
    static string lastError;

public:
    static bool saveToFile(const Scene& scene, const string& filePath);
    static string saveToString(const Scene& scene);
    static const string& getLastError();
};