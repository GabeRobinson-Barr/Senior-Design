#include "playergun.h"

PlayerGun::PlayerGun(Player* p) : GameObject(p->getPos(), p->getRot(), glm::vec3(1.f), 1.f, SPHERE)
{

}

PlayerGun::~PlayerGun()
{
    this->~GameObject();
}

void PlayerGun::update(float dt)
{
    GameObject::update(dt);
}

void PlayerGun::addForce(glm::vec3 force, glm::vec3 collPt)
{
    GameObject::addForce(force, collPt);
}

void PlayerGun::addCollision(GameObject* obj, glm::vec3 collisionPt)
{
    GameObject::addCollision(obj, collisionPt);
}
