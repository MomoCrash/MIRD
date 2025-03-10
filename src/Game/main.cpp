#include "main.h"

#include "Engine.h"
#include "RenderWindow.h"
#include "TestScene.h"
#include "GameScene.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    
    Engine::SetRenderWindow(hInstance);
    
    Engine::GetGameManager()->LaunchScene<GameScene>();
    
    return 0;
}
