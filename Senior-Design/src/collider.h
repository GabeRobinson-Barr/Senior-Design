#pragma once
#include "gameobject.h"

class Collider
{
public:
    Collider();
    ~Collider();

private:
    GameObject* collider1;
    GameObject* collider2;
    glm::vec3 collisionPos;

    bool isSticky;
    float collisionTime; // Time these objects have collided
};
