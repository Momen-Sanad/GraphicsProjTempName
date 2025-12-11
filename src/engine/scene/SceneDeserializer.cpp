//#include "SceneDeserializer.hpp"
//#include <json/single_include/nlohmann/json.hpp>
//#include <fstream>
//#include <iostream>
//
//using namespace std;
//using json = nlohmann::json;
//
//string SceneDeserializer::lastError;
//
//unique_ptr<Scene> SceneDeserializer::loadFromFile(const string& filePath) {
//    try {
//        ifstream file(filePath);
//        if (!file.is_open()) {
//            lastError = "Failed to open file: " + filePath;
//            return nullptr;
//        }
//
//        json j;
//        file >> j;
//        file.close();
//
//        auto scene = make_unique<Scene>();
//
//        // Parse scene name
//        if (j.contains("name")) {
//            scene->name = j["name"].get<string>();
//        }
//
//        // Parse settings
//        if (j.contains("settings")) {
//            const auto& settings = j["settings"];
//            if (settings.contains("clearColor")) {
//                scene->settings.clearColor = parseFloatArray(settings["clearColor"], 4);
//            }
//            if (settings.contains("cameraPosition")) {
//                scene->settings.cameraPosition = parseFloatArray(settings["cameraPosition"], 3);
//            }
//            if (settings.contains("cameraDirection")) {
//                scene->settings.cameraDirection = parseFloatArray(settings["cameraDirection"], 3);
//            }
//        }
//
//        // Parse entities
//        if (j.contains("entities")) {
//            for (const auto& entityJson : j["entities"]) {
//                Scene::EntityData entity;
//                
//                entity.name = entityJson.value("name", "");
//                entity.parent = entityJson.value("parent", "");
//                
//                if (entityJson.contains("position")) {
//                    entity.position = parseFloatArray(entityJson["position"], 3);
//                }
//                if (entityJson.contains("rotation")) {
//                    entity.rotation = parseFloatArray(entityJson["rotation"], 3);
//                }
//                if (entityJson.contains("scale")) {
//                    entity.scale = parseFloatArray(entityJson["scale"], 3);
//                }
//                
//                if (entityJson.contains("tint")) {
//                    entity.tint = parseFloatArray(entityJson["tint"], 4);
//                }
//                
//                // should add textures
//                // maybe all variables of textures like tinted material
//                // and/or textured material and so on
//                
//                scene->entities.push_back(move(entity));
//            }
//        }
//
//        return scene;
//    }
//    catch (const exception& e) {
//        lastError = "JSON parsing error: " + string(e.what());
//        return nullptr;
//    }
//}
//
//const string& SceneDeserializer::getLastError() {
//    return lastError;
//}
//
//vector<float> SceneDeserializer::parseFloatArray(const json& j, size_t expectedSize) {
//    vector<float> result;
//    
//    if (j.is_array()) {
//        for (const auto& element : j) {
//            if (element.is_number()) {
//                result.push_back(element.get<float>());
//            }
//        }
//    }
//
//    if (expectedSize > 0 && result.size() != expectedSize) {
//        lastError = "Float array has wrong size: expected " + to_string(expectedSize) + 
//                   ", got " + to_string(result.size());
//    }
//
//    return result;
//}