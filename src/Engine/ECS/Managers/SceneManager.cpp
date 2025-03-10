#include "pch.h"
#include "SceneManager.h"

SceneManager* SceneManager::Get()
{
   static SceneManager instance;

    return &instance;
}
    

