//#pragma once
//
//#include "Scene.hpp"
//#include <string>
//#include <memory>
//
//using namespace std;
//
///**
// * Simple JSON scene loader
// */
//class SceneDeserializer {
//private:
//    static string lastError;
//    static vector<float> parseFloatArray(const nlohmann::json& json, size_t expectedSize = 0);
//
//public:
//    static unique_ptr<Scene> loadFromFile(const string& filePath);
//    static const string& getLastError();
//
//};