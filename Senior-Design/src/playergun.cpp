#include "playergun.h"
#include "player.h"
#include <iostream>
using namespace std;

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
    if(!isFired)
    {
        //cout << "unfired" << endl;
        GameObject::updateTransform(myPlayer->getPos(), myPlayer->getRot(), scale);
        setVel(myPlayer->getVel());
        setRotVel(myPlayer->getRotVel());
        if(connectedComp != nullptr)
        {
            connectedComp->removeAll();
        }
    }
    else if(firing)
    {
        //cout << "fired pos: " << getPos().x << ", " << getPos().y << ", " << getPos().z << endl;
        float currLen = glm::length(getPos() - myPlayer->getPos());
        if(connectedComp != nullptr)
        {
            firing = false;
            ropeLen = currLen;
            //setVel(myPlayer->getVel());
            isAttached = true;
            //cout << "hit" << endl;
        }
        else if (currLen >= 500.f)
        {
            firing = false;
            ropeLen = currLen;
            setVel(glm::vec3(0));
            cout << "maxlen" << endl;
        }
    }
    else if(retracting)
    {
        //cout << "fired pos: " << getPos().x << ", " << getPos().y << ", " << getPos().z << endl;
        //cout << "player vel: " << myPlayer->getVel().x << ", " << myPlayer->getVel().y << ", " << myPlayer->getVel().z << endl;
        ropeLen -= pullSpd * dt;
        glm::vec3 currVec = getPos() - myPlayer->getPos();
        if(detached)
        {
            setVel(pullSpd * glm::normalize(myPlayer->getPos() - getPos()));
        }
        else if(ropeLen <= glm::length(currVec))
        {
            currVec = glm::normalize(currVec);
            glm::vec3 spdVec = -getVel() + myPlayer->getVel();
            float currSpd = 0.f;
            if(glm::length(spdVec) > 0.f)
            {
                currSpd = glm::length(spdVec) * glm::dot(currVec, glm::normalize(spdVec));
            }
            if(currSpd < pullSpd)
            {
                glm::vec3 spdDiff = (pullSpd * currVec) - spdVec;
                if(connectedComp != nullptr && !connectedComp->isDynamic)
                {
                    myPlayer->addForce(spdDiff, getPos());
                }
                else if(isAttached)
                {
                    addForce(-spdDiff / getMass(), myPlayer->getPos());
                    myPlayer->addForce(spdDiff / myPlayer->getMass(), getPos());
                }
                else
                {
                    setVel(pullSpd * glm::normalize(myPlayer->getPos() - getPos()));
                }
                ropeLen = glm::length(getPos() - myPlayer->getPos());
            }
        }
    }
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
    translate(castVec * 1.5f);
    setVel(glm::vec3(0));
    setRotVel(glm::vec3(0));
    addForce(glm::normalize(castVec) * fireSpd);
    this->castVec = castVec;
}

void PlayerGun::retract(glm::vec3 playerPos)
{
    if(isFired && !retracting)
    {
        //cout << "retracting" << endl;
        firing = false;
        retracting = true;
        disableOne();
        glm::vec3 retVec = getPos() - playerPos;
        translate(glm::normalize(-retVec) * fireSpd * (16.f / 1000.f));
    }

}

void PlayerGun::playerCollision()
{
    if(isFired)
    {
        //cout << "hitplayer" << endl;
        detached = false;
        isFired = false;
        firing = false;
        retracting = false;
        isAttached = false;
        setVel(glm::vec3(0));
        setRotVel(glm::vec3(0));

        // when this returns to the player destroy its connected objects
        if(connectedComp != nullptr)
        {
            connectedComp->removeAll();
        }
    }
}

void PlayerGun::detach()
{
    if(isAttached)
    {
        detached = true;
        if(connectedComp != nullptr)
        {
            connectedComp->removeObj(this);
        }
        retract(myPlayer->getPos());
    }
}

glm::vec4 PlayerGun::getColor()
{
    return glm::vec4(1.f, 1.f, 1.f, 1.f);
}

void PlayerGun::respawn()
{
    isFired = false;
    firing = false;
    retracting = false;
    isAttached = false;
    detached = false;
    if(connectedComp != nullptr)
    {
        connectedComp->removeAll();
    }
    updateTransform(myPlayer->getPos(), myPlayer->getRot(), scale);
    pos = myPlayer->getPos();
    rot = myPlayer->getRot();
    setVel(glm::vec3(0));
    setRotVel(glm::vec3(0));
    lastTransform = m_transform;
}
