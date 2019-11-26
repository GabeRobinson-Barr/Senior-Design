#include "playergun.h"
#include "player.h"

PlayerGun::PlayerGun(Player* p) : GameObject(p->getPos(), p->getRot(), glm::vec3(0.2f,0.4f,0.2f), 1.f, SPHERE)
{
    this->isSticky = true;
    myPlayer = p;
}

PlayerGun::~PlayerGun()
{
    this->GameObject::~GameObject();
}

void PlayerGun::update(float dt)
{
    GameObject::update(dt);
    if(connectedComp != nullptr && firing)
    {
        firing = false;
        ropeLen = glm::length(getPos() - myPlayer->getPos());
    }
    if(retracting)
    {
        ropeLen -= fireSpd * dt;
        glm::vec3 currVec = getPos() - myPlayer->getPos();
        if(ropeLen <= glm::length(currVec))
        {
            //float lenDiff = glm::length(currVec) - ropeLen;
            //translate(-lenDiff * glm::normalize(currVec));
            float totMass = getMass() + myPlayer->getMass();
            //setVel()
            /*glm::vec3 velDiff = (-fireSpd * myPlayer->getMass() * glm::normalize(currVec)) - getVel();
            addForce(velDiff, myPlayer->getPos());
            glm::vec3 playerVelDiff = (fireSpd * myPlayer)
            myPlayer->addForce(fireSpd * totMass * glm::normalize(currVec), getPos());*/
        }
    }
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
