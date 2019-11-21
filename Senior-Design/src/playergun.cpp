#include "playergun.h"
#include "player.h"

PlayerGun::PlayerGun(Player* p) : GameObject(p->getPos(), p->getRot(), glm::vec3(0.2f,0.4f,0.2f), 1.f, SPHERE)
{
    this->isSticky = true;

}

PlayerGun::~PlayerGun()
{
    this->GameObject::~GameObject();
}

void PlayerGun::update(float dt)
{
    GameObject::update(dt);
}

void PlayerGun::addForce(glm::vec3 force, glm::vec3 collPt)
{
    GameObject::addForce(force, collPt);
}

void PlayerGun::addForce(glm::vec3 force)
{
    glm::vec3 forceVec = force / (16.f / 1000.f);
    forces += forceVec;
}

void PlayerGun::addCollision(GameObject* obj, glm::vec3 collisionPt)
{
    // this shouldnt happen because the gun is always sticky
    return;
}

void PlayerGun::fire(glm::vec3 castVec, glm::vec3 castPos)
{
    if(isFired) // make sure we havent already fired the gun
    {
        return;
    }
    isFired = true;
    firing = true;
    addForce(glm::normalize(castVec) * fireSpd);
    this->castVec = castVec;
}

void PlayerGun::retract(glm::vec3 playerPos)
{
    retracting = true;
    addForce(glm::normalize(getPos() - playerPos) * fireSpd, playerPos);
}

void PlayerGun::playerCollision()
{
    isFired = false;
    retracting = false;
    isAttached = false;
    // when this returns to the player destroy its connected objects
    if(connectedComp != nullptr)
    {
        connectedComp->removeAll();
    }
}
