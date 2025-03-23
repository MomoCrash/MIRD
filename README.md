# MIRD : Moteur Incroyable de Rendu DirectX12

> Directly download the [lasted release](https://github.com/MomoCrash/MIRD/releases/), unzip, and test the engine or just read the code ! \
> Or you can download the repository to read code.

# About 

MIRD is a 3D engine written in C++ using DirectX, structured into two main modules.

Rendering module: Manage PSOs, shaders, and compute shaders (for particles) and other "basics" DirectX12 initialisations.
Engine module: Built on an ECS (Entity Component System) with advanced scripting management inspired by Unity.

### Key aspects
- The 3D rendered part is not restricted to our engine it is easily usable. It's made to be used in anyone's engine.
- Easy creation of entity and component attachment for fast and easy usage.

```cpp
// entity creation
Entity* player = ObjectFactory::CreateEntity<Entity>();
player->GetTransform()->SetPosition(0.0f, 150.0f, 0.0f);
// Add your components
ObjectFactory::CreateComponent<RigidBody3D>(player);
ObjectFactory::CreateComponent<BoxCollider>(player, -1.0f, -5.0f, -1.0f, 1.0f, 5.0f, 1.0f, false, false);
// (Optional) Define a TAG
player->SetTag(Entity::Tag::PLAYER);
// Attach scripts
ObjectFactory::AttachScript<PlayerMovement>(player);
ObjectFactory::AttachScript<PlayerShoot>(player);
```

# Build the solution 
1. Clone the repository
4. Go to ./bin
5. Run `make.bat`
6. Run the Mithaniel.sln

> Made by Fabio Alves, Ethan Gilotin, Liam Guillermont
