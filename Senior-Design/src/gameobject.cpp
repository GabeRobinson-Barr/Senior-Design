#include "gameobject.h"

GameObject::GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, Drawable* geom) :
    pos(pos), rot(rot), scale(scale), mass(mass), mp_geometry(geom)
{
    //TODO:
    //CREATE COLLIDER
    //create matrices
}

GameObject::~GameObject()
{

}

void GameObject::update(float dt)
{

}

void GameObject::addForce(glm::vec3 force)
{

}

void GameObject::getForce()
{

}
