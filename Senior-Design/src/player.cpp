#include "player.h"
#include "QKeyEvent"
#include <iostream>
using namespace std;

Player::Player(Camera** c) : GameObject(glm::vec3(0,15,0), glm::vec3(0,0,0), glm::vec3(1,2,1), 1.f, SPHERE),
    camOffset(glm::vec3(0,0.9f,0.55f)), cam(*c), currBoost(60.f), maxBoost(60.f), jumpStr(15.f),
    moveSpd(5.f), rotSpd(60.f)
{
    isSticky = false;
    cam = new Camera((*c)->width, (*c)->height, pos + camOffset, pos + glm::vec3(0,0,1), (*c)->world_up);
    *c = cam;
    drag = 0.02f;
    recomputeAttributes();
    //delete c;
}

Player::~Player(){
    delete cam;
}

void Player::update(float dt)
{
    updated = false;
    if(pos.y <= 0.f || connectedComp != nullptr || floorObj != nullptr)
    {
        onFloor = true;
    }
    glm::vec3 moveDir;
    glm::vec3 currVel = getVel();
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
    if (rotRight) {
        rot.y -= rotSpd * dt;
    }
    if (rotLeft) {
        rot.y += rotSpd * dt;
    }
    if(glm::length(moveDir) >= 0.0001f && glm::length(playerSpd) < moveSpd * dt)
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
    if(onFloor && jumped)
    {
        translate(glm::vec3(playerUp * jumpStr * dt * 2.f));
        addForce(playerUp * jumpStr);

        if(floorObj != nullptr)
        {
            glm::vec3 jumpPt = pos + -playerUp;
            floorObj->addForce(-playerUp * jumpStr, jumpPt);

        }
    }
    if(!onFloor)
    {
        //cout << "not grounded" << endl;
        addForce(-playerUp * playerGrav * mass);
    }
    else
    {
        //vel.x = 0.f;
        vel.y = max(vel.y, 0.f);
        //vel.z = 0.f;
        //rotVel = glm::vec3(0);
    }
    //cout << "Force y: " << forces.y << endl;

    this->GameObject::update(dt);
    cam->eye = pos + glm::vec3(m_transform.rotMat() * glm::vec4(0,1,0.5f,1));
    cam->ref = cam->eye + glm::vec3(m_transform.rotMat() * glm::vec4(0,0,1,1));
    cam->RecomputeAttributes();
    recomputeAttributes();
    cout << "Eye: " << cam->eye.x << ", " << cam->eye.y << ", " << cam->eye.z << '\n';
    cout << "Ref: " << cam->ref.x << ", " << cam->ref.y << ", " << cam->ref.z << '\n';
    cout << "Camup: " << cam->up.x << ", " << cam->up.y << ", " << cam->up.z << '\n';
    cout << "playerUp" << playerUp.x << ", " << playerUp.y << ", " << playerUp.z << endl;
    onFloor = false;
    jumped = false;
}

void Player::addForce(glm::vec3 force, glm::vec3 collPt)
{
    GameObject::addForce(force, collPt);
}

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
    glm::vec3 collNor = -getNor(collisionPt - obj->getPos());
    if(collNor.y <= 0.f)
    {
        onFloor = true;
        floorObj = obj;
    }

}

void Player::recomputeAttributes()
{
    playerUp = glm::vec3(m_transform.rotMat() * glm::vec4(cam->world_up,1));
    playerFor = glm::vec3(m_transform.rotMat() * glm::vec4(0,0,1,1));
    playerRight = glm::cross(playerFor, playerUp);
}
