#pragma once
#include <vector>
#include <map>
#include <string>
#include<iostream>

class Scene;

class SceneManager
{
public:
    ~SceneManager() = default;
    SceneManager() = default;
    static SceneManager* Get();

    template <typename T>
    void AddScene(std::string name, Scene* scene);

    template <typename T>
    Scene* GetScene();

protected:
    Scene* mActiveScene;
    std::map<std::string,Scene*> mMapScene;
};
    
// template <typename T>
// void SceneManager::AddScene(std::string name, Scene* scene)
// {
//     std::is_base_of<Scene, T>::value;
//     T* scene = new(this);
//     mMapScene[name] = std::move(scene);
// }

template <typename T>
Scene* SceneManager::GetScene()
{
    for (Scene* scene : mActiveScene)
    {
        if (T* foundScene = dynamic_cast<T*>(scene))
        {
            return foundScene;
        }
    }
    return nullptr;
}