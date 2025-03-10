#include "TestCollision.h"

void TestCollision::OnTriggerEnter(Entity* other)
{
    std::cout << "yes" << std::endl;
}

void TestCollision::OnTriggerStay(Entity* other)
{
    std::cout << "nn" << std::endl;
}

void TestCollision::OnTriggerExit(Entity* other)
{
    std::cout << "maybe" << std::endl;
}
