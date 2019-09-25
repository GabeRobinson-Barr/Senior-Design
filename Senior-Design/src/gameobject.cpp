#include "gameobject.h"

GameObject::GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type) :
    m_transform(Transform(pos, rot, scale)), mass(mass), geomType(type)
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

glm::vec3 GameObject::getForce()
{

}

glm::vec3 GameObject::getPos()
{
    return m_transform.position();
}

glm::mat4 GameObject::getTransform()
{
    return m_transform.T();
}
