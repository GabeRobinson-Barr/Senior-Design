#include "player.h"
#include "QKeyEvent"
#include <iostream>
using namespace std;

Player::Player(Camera** c) : GameObject(glm::vec3(0,15,0), glm::vec3(0,0,0), glm::vec3(1,2,1), 1.f, SPHERE),
    camOffset(glm::vec3(0,0.9f,0.55f)), cam(*c), currBoost(60.f), maxBoost(60.f), jumpStr(40.f),
    maxMoveSpd(5.f), rotSpd(60.f), myGun(PlayerGun(this))
{
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
        if(pos.y <= 0.f || connectedComp != nullptr || floorObj != nullptr)
        {
            onFloor = true;
        }
        glm::vec3 moveDir = getMoveDir();
        glm::vec3 currVel = getVel();

        if (rotRight) {
            rot.y -= rotSpd * dt;
        }
        if (rotLeft) {
            rot.y += rotSpd * dt;
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
        updateTransform();
        if(onFloor)
        {
            if(jumped)
            {
                translate(glm::vec3(playerUp * jumpStr * dt * 2.f));
                addForce(playerUp * jumpStr);

                if(floorObj != nullptr)
                {
                    glm::vec3 jumpPt = pos + -playerUp;
                    floorObj->addForce(-playerUp * jumpStr, jumpPt);
                    floorObj = nullptr;
                    onFloor = false;
                }
            }
            //cout << "movedir: " << moveDir.x << ", " << moveDir.z << endl;
            //cout << "Force: " << forces.x << ", " << forces.z << endl;
            this->GameObject::update(dt);
            vel.y = max(vel.y, 0.f);
            if(floorObj != nullptr) // check if we are still on the floor after updating
            {
                std::pair<bool,glm::vec3> p = collide(this, floorObj);
                if(!p.first)
                {
                    floorObj = nullptr;
                    onFloor = false;
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
    cam->eye = pos + glm::vec3(m_transform.rotMat() * glm::vec4(0,1,0.5f,1));
    cam->ref = cam->eye + glm::vec3(m_transform.rotMat() * glm::vec4(0,0,1,1));
    cam->RecomputeAttributes();
    recomputeAttributes();
    //cout << "Eye: " << cam->eye.x << ", " << cam->eye.y << ", " << cam->eye.z << '\n';
    //cout << "Ref: " << cam->ref.x << ", " << cam->ref.y << ", " << cam->ref.z << '\n';
    //cout << "Camup: " << cam->up.x << ", " << cam->up.y << ", " << cam->up.z << '\n';
    //cout << "playerUp" << playerUp.x << ", " << playerUp.y << ", " << playerUp.z << endl;
    onFloor = false;
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
        } /*else if (e->key() == Qt::Key_Up) {
            key_Up = true;
        } else if (e->key() == Qt::Key_Down) {
            key_Down = true;
        } else if (e->key() == Qt::Key_1) {
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
        } /*else if (e->key() == Qt::Key_Up) {
            key_Up = true;
        } else if (e->key() == Qt::Key_Down) {
            key_Down = true;
        } else if (e->key() == Qt::Key_1) {
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
    GameObject::addCollision(obj, collisionPt);
    glm::vec3 collVec = collisionPt - getPos();
    //cout << "collVec: " << collVec.x << ", " << collVec.y << ", " << collVec.z << endl;
    if(collVec.y < -0.0001f)
    {
        onFloor = true;
        floorObj = obj;
        //vel.y = 0.f;
        //forces.y = 0.f;
        cout << "hasfloor" << endl;
    }

}

void Player::recomputeAttributes()
{
    playerUp = glm::vec3(m_transform.rotMat() * glm::vec4(cam->world_up,1));
    playerFor = glm::vec3(m_transform.rotMat() * glm::vec4(0,0,1,1));
    playerRight = glm::cross(playerFor, playerUp);
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
