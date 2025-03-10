#include <random>

#include "TileScript.h"

#include "Bumper.h"
#include "EnemyAction.h"
#include "Resources.h"
#include "Transform.h"
#include "ECS/ObjectFactory.h"
#include "ECS/Components/Colliders/BoxCollider.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Colliders/SphereCollider.h"

void TileScript::OnStart()
{
    Resources* resources = &Resources::instance();

    std::random_device rdBumper; 
    std::mt19937 genBumper(rdBumper());
    std::uniform_int_distribution<> iDistribution(0, 5); 
    int result = iDistribution(genBumper);

    if(result == 0)
    {
        std::uniform_int_distribution<> iPosX(-3, 3);
        int resultPosX = iPosX(genBumper);
        
        std::uniform_int_distribution<> iPosZ(-3, 3);
        int resultPosZ = iPosZ(genBumper);

        Entity* bumper = ObjectFactory::CreateEntity<Entity>();
        bumper->GetTransform()->SetScale({ 2.5f, 0.5f, 2.5f, 1.0f });
        bumper->GetTransform()->SetPosition(owner->GetTransform()->position.x + resultPosX, owner->GetTransform()->position.y + 5.0f, owner->GetTransform()->position.z + resultPosZ);
        ObjectFactory::CreateComponent<MeshRenderer>(bumper, GeometryFactory::GetCube(), resources->MATERIAL_BOUCEPAD);
        BoxCollider* bumperCollider = ObjectFactory::CreateComponent<BoxCollider>(bumper, -2.5f, -2.0f, -2.5f, 2.5f, 2.0f, 2.5f, true, true);
        ObjectFactory::AttachScript<Bumper>(bumper);
    }
    std::uniform_int_distribution<> iEnemyRand(0, 10); 
    result = iEnemyRand(genBumper);
    if (result == 1)
    {
        std::uniform_int_distribution<> iPosX(-3, 3);
        int resultPosX = iPosX(genBumper);
        
        std::uniform_int_distribution<> iPosZ(-3, 3);
        int resultPosZ = iPosZ(genBumper);
        
        Entity* enemyEye = ObjectFactory::CreateEntity<Entity>();
        enemyEye->GetTransform()->SetPosition(owner->GetTransform()->position.x + resultPosX, owner->GetTransform()->position.y + 10.0f, owner->GetTransform()->position.z + resultPosZ);
        enemyEye->GetTransform()->SetScale({ 1.5f, 1.5f, 1.5f });
        ObjectFactory::CreateComponent<MeshRenderer>(enemyEye, Resources::instance().MESH_EYE, resources->MATERIAL_EYE);
        ObjectFactory::CreateComponent<SphereCollider>(enemyEye, 1.0f, false, true);
        enemyEye->SetTag(Entity::Tag::ENNEMY);
        ObjectFactory::AttachScript<EnemyAction>(enemyEye);

        Entity* triggerBox = ObjectFactory::CreateEntity<Entity>(enemyEye);
        triggerBox->GetTransform()->SetScale({ 1.0f, 1.0f, 1.0f });
        ObjectFactory::CreateComponent<SphereCollider>(triggerBox, 30.0f, true, true);
        triggerBox->SetTag(Entity::Tag::ENNEMY);
    
        ObjectFactory::AttachScript<EnemyTrigger>(triggerBox);
    }
}
