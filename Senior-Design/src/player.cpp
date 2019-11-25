#include "player.h"
#include "QKeyEvent"
#include <iostream>
using namespace std;

Player::Player(Camera** c) : GameObject(glm::vec3(0,20,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1.f, SPHERE),
    camOffset(glm::vec3(0,0,0.5f)), cam(*c), currBoost(60.f), maxBoost(60.f), jumpStr(40.f),
    maxMoveSpd(5.f), rotSpd(60.f), myGun(PlayerGun(this))
{
    camRot = getRot();
    isSticky = false;
    cam = new Camera((*c)->width, (*c)->height, pos + camOffset, pos + glm::vec3(0,0,1), (*c)->world_up);
    *c = cam;
    drag = 0.02f;
    recomputeAttributes();
}

Player::~Player(){
    delete cam;
}

void Player::update(float dt)
{
    updated = false;
    if(tempStop)
    {
        tempStop = false;
    }
    else
    {
        if( connectedComp != nullptr || floorObj != nullptr)
        {
            onFloor = true;
            //cout << "hasfloor" << endl;
        }
        else
        {
            //cout << "hasnofloor" << endl;
        }
        glm::vec3 moveDir = getMoveDir();
        glm::vec3 currVel = getVel();

        if (rotRight) {
            nextRot.y -= rotSpd * dt;
        }
        if (rotLeft) {
            nextRot.y += rotSpd * dt;
        }
        if (rotUp) {
            if(camRot.x >= -80.f)
            {
                camRot.x -= camRotSpd * dt;
            }
        }
        if (rotDown) {
            if (camRot.x <= 80.f)
            {
                camRot.x += camRotSpd * dt;
            }
        }
        if(glm::length(moveDir) >= 0.0001f && glm::length(playerSpd) < maxMoveSpd * dt)
        {
            glm::vec3 pForce = glm::normalize(moveDir) * speedInc;
            addForce(pForce);
            playerSpd = playerSpd + (pForce / getMass()) * dt;
        }
        else if (glm::length(getVel()) >= 0.0001f)
        {
            glm::vec3 dragVel = -getVel();
            dragVel.y = 0.f;
            addForce(dragVel * drag);
            playerSpd = playerSpd - (playerSpd / getMass()) * drag;
        }

        if(onFloor)
        {
            nextVel.y = max(nextVel.y, 0.f);
            if(jumped)
            {
                //cout << "jumped" << endl;
                translate(glm::vec3(playerUp * jumpStr * dt * 2.f));
                addForce(playerUp * jumpStr);

                if(floorObj != nullptr)
                {
                    if(floorObj->isDynamic)
                    {
                        glm::vec3 jumpPt = getPos() + -playerUp;
                        floorObj->addForce(-playerUp * jumpStr, jumpPt);
                    }
                    floorObj = nullptr;
                    onFloor = false;
                }
            }
            //cout << "movedir: " << moveDir.x << ", " << moveDir.z << endl;
            //cout << "Force: " << forces.x << ", " << forces.z << endl;
            this->GameObject::update(dt);
            if(floorObj != nullptr) // check if we are still on the floor after updating
            {
                std::pair<bool,glm::vec3> p = collide(this, floorObj);
                if(!p.first)
                {
                    floorObj = nullptr;
                    onFloor = false;
                    //cout << "lost Floor" << endl;
                }
            }
        }
        else
        {
            //cout << "not grounded" << endl;
            addForce(-playerUp * playerGrav * mass);
            this->GameObject::update(dt);
        }
    }
    //cout << "Force y: " << forces.y << endl;
    //cout << "Vel y: " << getVel().y << endl;
    recomputeCam();
    recomputeAttributes();
    cout << "Camrot X: " << camRot.x << endl;
    cout << "Eye: " << cam->eye.x << ", " << cam->eye.y << ", " << cam->eye.z << '\n';
    cout << "Ref: " << cam->ref.x << ", " << cam->ref.y << ", " << cam->ref.z << '\n';
    cout << "Camup: " << cam->up.x << ", " << cam->up.y << ", " << cam->up.z << '\n' << endl;
    //cout << "playerUp" << playerUp.x << ", " << playerUp.y << ", " << playerUp.z << endl << endl;
    jumped = false;
}

void Player::addForce(glm::vec3 force, glm::vec3 collPt)
{
    GameObject::addForce(force, collPt);
}

// this one handles player input forces
void Player::addForce(glm::vec3 force)
{
    glm::vec3 forceVec = force / (16.f / 1000.f);
    forces += forceVec;
}

void Player::keyPressed(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Right) {
            rotRight = true;
        } else if (e->key() == Qt::Key_Left) {
            rotLeft = true;
        } else if (e->key() == Qt::Key_Up) {
            rotUp = true;
        } else if (e->key() == Qt::Key_Down) {
            rotDown = true;
        } /*else if (e->key() == Qt::Key_1) {
            key_1 = true;
        } else if (e->key() == Qt::Key_2) {
            key_2 = true;
        }*/ else if (e->key() == Qt::Key_W) {
            moveFor = true;
        } else if (e->key() == Qt::Key_S) {
            moveBack = true;
        } else if (e->key() == Qt::Key_D) {
            moveRight = true;
        } else if (e->key() == Qt::Key_A) {
            moveLeft = true;
        } else if (e->key() == Qt::Key_Space) {
        jumped = true;
    }
}

void Player::keyReleased(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Right) {
            rotRight = false;
        } else if (e->key() == Qt::Key_Left) {
            rotLeft = false;
        } else if (e->key() == Qt::Key_Up) {
            rotUp = false;
        } else if (e->key() == Qt::Key_Down) {
            rotDown = false;
        } /*else if (e->key() == Qt::Key_1) {
            key_1 = true;
        } else if (e->key() == Qt::Key_2) {
            key_2 = true;
        }*/ else if (e->key() == Qt::Key_W) {
            moveFor = false;
        } else if (e->key() == Qt::Key_S) {
            moveBack = false;
        } else if (e->key() == Qt::Key_D) {
            moveRight = false;
        } else if (e->key() == Qt::Key_A) {
            moveLeft = false;
        }
}

void Player::addCollision(GameObject* obj, glm::vec3 collisionPt)
{
    glm::vec3 lastNor = obj->getNor(getLastTransform().position() - obj->getLastTransform().position());
    if(!obj->isDynamic)
    {
        glm::vec3 collVec = obj->getSupport(collisionPt);
        collVec = glm::vec3(obj->getTransform().rotMat() * glm::vec4(collVec,1)) * obj->getScale();
        //cout << "collvec" << collVec.x << ", " << collVec.y << ", " << collVec.z << endl;
        glm::vec3 posDiff = glm::vec3(0.f);
        glm::vec3 velDiff = getVel();
        //cout << "Vel y: " << getVel().y * (16.f / 1000.f) << endl;
        for(int i = 0; i < 3; i++)
        {
            if(glm::abs(lastNor[i]) > 0.f && glm::abs(collVec[i]) < 0.5f)
            {
                posDiff[i] = lastNor[i] - collVec[i];
                if(lastNor[i] > 0.f)
                {
                    velDiff[i] = max(0.f, velDiff[i]);
                }
                else
                {
                    velDiff[i] = min(0.f, velDiff[i]);
                }
            }
        }
        translate(posDiff);
        setVel(velDiff);
        //cout << "posDiff" << posDiff.x << ", " << posDiff.y << ", " << posDiff.z << endl;
        //cout << "lastNor" << lastNor.x << ", " << lastNor.y << ", " << lastNor.z << endl;
    }
    if(lastNor.y > 0.f)
    {
        //cout << "found floor" << endl;
        onFloor = true;
        floorObj = obj;
        //nextVel.y = max(0.f, getVel().y);
        //vel.y = 0.f;
        //forces.y = 0.f;
    }
    else if(obj == floorObj)
    {
        onFloor = false;
        floorObj = nullptr;
    }
    GameObject::addCollision(obj, collisionPt);
}

void Player::recomputeAttributes()
{
    playerUp = glm::vec3(lastTransform.rotMat() * glm::vec4(cam->world_up,1));
    playerFor = glm::vec3(lastTransform.rotMat() * glm::vec4(0,0,1,1));
    playerRight = glm::cross(playerFor, playerUp);
}

void Player::recomputeCam()
{
    cam->eye = pos + glm::vec3(lastTransform.rotMat() * glm::vec4(camOffset,1));
    glm::vec3 refVec = glm::vec3(lastTransform.rotMat() * glm::rotate(glm::mat4(), glm::radians(camRot.x), glm::vec3(1,0,0)) *
                                 glm::vec4(0,0,1,1));
    cam->ref = cam->eye + refVec;
    cam->RecomputeAttributes();
}

bool Player::objIsFloor(GameObject* obj)
{
    return floorObj != nullptr && floorObj == obj;
}

glm::vec3 Player::getMoveDir()
{
    glm::vec3 moveDir = glm::vec3(0.f);
    if (moveFor) {
        moveDir += playerFor;
    }
    if (moveBack) {
        moveDir -= playerFor;
    }
    if (moveRight) {
        moveDir += playerRight;
    }
    if (moveLeft) {
        moveDir -= playerRight;
    }

    if(glm::length(moveDir) > 0.0001f)
    {
        return glm::normalize(moveDir);
    }
    else return moveDir;
}

float Player::getMoveSpeed()
{
    return speedInc;
}

void Player::dynamicCollide()
{
    tempStop = true;
}

void Player::setFloor(GameObject *obj)
{
    onFloor = true;
    floorObj = obj;
}
