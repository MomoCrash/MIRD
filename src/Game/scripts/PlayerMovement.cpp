#include "PlayerMovement.h"

#include "RenderWindow.h"
#include "InputManager/InputManager.h"
#include "InputManager/Keycode.h"
#include "Transform.h"
#include "ECS/Components/RigidBody3D.h"

void PlayerMovement::OnStart()
{
    rigidbody = owner->GetComponent<RigidBody3D>();
    camera = owner->GetTransform()->GetChildList()[0];
    phi = 0.0f;
}

void PlayerMovement::OnFixedUpdate()
{
    float dt = Engine::GetGameManager()->GetTime()->GetFixedDeltaTime();
    XMVECTOR velocity = XMVectorAbs(rigidbody->GetForce());
    if (XMVectorGetX(velocity) < 4.0f || XMVectorGetZ(velocity) < 4.0f)
    {
        rigidbody->AddForce(XMLoadFloat3(&mDirection) * 1000.0f * dt);
    }
    XMStoreFloat3(&mDirection, XMVectorZero());


    // Calcul du mouvement de la souris par rapport au centre puis la remettre au centre
    TRANSFORM* transform = owner->GetTransform(); 
    RECT rect;
    GetClientRect(Engine::GetRenderWindow()->GetHwnd(), &rect);
    POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
    
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(Engine::GetRenderWindow()->GetHwnd(), &mousePos);

    float deltaX = static_cast<float>(mousePos.x - center.x);
    float deltaY = static_cast<float>(mousePos.y - center.y);

    float sensitivity = 0.20f;

    float dY = deltaY * dt * sensitivity;
    float dX = deltaX * dt * sensitivity;

    phi += dY;
    theta += dX;

    if (phi > 1.5f)
    {
        phi = 1.5f;
    } else if (phi < -1.5f) {
        phi = -1.5f;
    }
    XMVECTOR rotation = XMVectorSet(phi, theta, 0.0f, 1.0f);
    transform->SetLocalRotation(rotation);
    camera->GetTransform()->SetLocalRotation(rotation);
    
    ClientToScreen(Engine::GetRenderWindow()->GetHwnd(), &center);
    SetCursorPos(center.x, center.y);

    if (XMVectorGetY(XMLoadFloat3(&transform->position)) < -20.0f)
    {
        transform->SetPosition(10.0f, 50.0f, 10.0f);
        rigidbody->ClearForces();
        rigidbody->ClearVelocity();
    }
}

void PlayerMovement::OnPreRender()
{
    
}

void PlayerMovement::OnUpdate()
{
    
    TRANSFORM* transform = owner->GetTransform();
    XMVECTOR cForward = XMLoadFloat3(&transform->forward);
    XMVECTOR cRight = XMLoadFloat3(&transform->right);
    XMVECTOR direction = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (Engine::GetInputManager()->IsKeyPressed(KEY_Z))
    {
        direction = XMVectorAdd(direction, cForward);
    }

    if (Engine::GetInputManager()->IsKeyPressed(KEY_S))
    {
        direction = XMVectorAdd(direction, -cForward);
    }
	
    if (Engine::GetInputManager()->IsKeyPressed(KEY_Q))
    {
        direction = XMVectorAdd(direction, -cRight);
    }
	
    if (Engine::GetInputManager()->IsKeyPressed(KEY_D))
    {
        direction = XMVectorAdd(direction, cRight);
    }

    if (Engine::GetInputManager()->IsKeyPressed(KEY_SHIFT))
    {
        direction = XMVectorAdd(direction, direction * 2.0f);
    }
    
    if (Engine::GetInputManager()->IsKeyJustPressed(KEY_SPACE))
    {
        rigidbody->AddForce(XMVectorSet(0.0f, 1000.0f, 0.0f, 0.0f));
    }
    
    if (Engine::GetInputManager()->IsKeyJustPressed(KEY_1))
    {
        Engine::GetGameManager()->GetTime()->SetTimeScale(0.1f);
    }
    
    if (Engine::GetInputManager()->IsKeyJustPressed(KEY_2))
    {
        Engine::GetGameManager()->GetTime()->SetTimeScale(1.0f);
    }

    XMStoreFloat3(&mDirection, direction);
}

void PlayerMovement::OnDisable()
{
    
}
