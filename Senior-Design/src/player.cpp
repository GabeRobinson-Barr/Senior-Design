#include "player.h"
#include "QKeyEvent"
#include <iostream>
using namespace std;

Player::Player(Camera** c) : GameObject(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,2,1), 1.f, CUBE),
    camOffset(glm::vec3(0,0.9f,0.55f)), cam(*c), currBoost(60.f), maxBoost(60.f), jumpStr(10.f),
    moveSpd(5.f), rotSpd(60.f)
{
    isSticky = true;
    cam = new Camera((*c)->width, (*c)->height, pos + camOffset, pos + glm::vec3(0,0,1), (*c)->world_up);
    *c = cam;
    //delete c;
}

Player::~Player(){
    delete cam;
}

void Player::update(float dt)
{
    if(pos.y <= 0.f)
    {
        onFloor = true;
    }
    glm::vec3 moveDir;
    if (moveFor) {
        moveDir += cam->look;
    }
    if (moveBack) {
        moveDir -= cam->look;
    }
    if (moveRight) {
        moveDir += cam->right;
    }
    if (moveLeft) {
        moveDir -= cam->right;
    }
    if (rotRight) {
        rot.y -= rotSpd * dt;
    }
    if (rotLeft) {
        rot.y += rotSpd * dt;
    }
    if(glm::length(moveDir) >= 0.0001f)
    {
        translate(glm::normalize(moveDir) * moveSpd * dt);
    }
    if(onFloor && jumped)
    {
        glm::vec3 jumpPt = pos + -cam->up;
        addForce(cam->up * jumpStr, jumpPt);
        if(floorObj != nullptr)
        {
            floorObj->addForce(-cam->up * jumpStr, jumpPt);
        }
    }
    if(!onFloor)
    {
        addForce(-cam->up * 0.5f, pos + cam->up);
    }
    else
    {
        vel.x = 0.f;
        vel.y = max(vel.y, 0.f);
        vel.z = 0.f;
        rotVel = glm::vec3(0);
    }
    this->GameObject::update(dt);
    cam->eye = pos + glm::vec3(m_transform.rotMat() * glm::vec4(0,1,0.5f,1));
    cam->ref = cam->eye + glm::vec3(m_transform.rotMat() * glm::vec4(0,0,1,1));
    cam->RecomputeAttributes();
    cout << "Eye: " << cam->eye.x << ", " << cam->eye.y << ", " << cam->eye.z << '\n';
    cout << "Ref: " << cam->ref.x << ", " << cam->ref.y << ", " << cam->ref.z << '\n';
    cout << "Camup: " << cam->up.x << ", " << cam->up.y << ", " << cam->up.z << '\n';
    onFloor = false;
    jumped = false;
}

void Player::addForce(glm::vec3 force, glm::vec3 collPt)
{
    if(getNor(collPt - pos).y - (-1.f) <= 0.0001f)
    {
        onFloor = true;
    }
    GameObject::addForce(force, collPt);
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
