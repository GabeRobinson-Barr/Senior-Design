#include "gameobject.h"

GameObject::GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type) :
    pos(pos), rot(rot), scale(scale), m_transform(Transform(pos, rot, scale)),
    vel(glm::vec3(0.f)),  rotVel(glm::vec3(0.f)), mass(mass), drag(1.f), angDrag(1.f),
    forces(glm::vec3(0.f)), geomType(type)
{
    //TODO:
    //CREATE COLLIDER
    //create matrices
}

GameObject::~GameObject()
{

}

void GameObject::collide(GameObject *obj1, GameObject *obj2)
{
    bool collided = false;
    // ONLY SPHERES IMPLEMENTED RN
    if(obj1->geomType == SPHERE && obj2->geomType == SPHERE) // simple sphere check
    {
        glm::vec3 diff = obj1->getPos() - obj2->getPos();
        float collisionDist = (obj1->scale.x + obj2->scale.x) / 2.f;
        if(glm::length(diff) <= collisionDist)
        {
            collided = true;
        }
    }
    else
    {
        // TODO: TRY TO IMPLEMENT GJK
        // GET THE MINOWSKI DIFF OF THE OBJECTS (USE SUPPORT MAPPING FOR THIS)
        // FIND THE SUPPORT POINT TO THE ORIGEN
        // DO THE SIMPLEX THINGY WHERE YOU ITERATIVELY FIND THE CLOSEST POINT

        /* take closest (support) points to other's origin
            get minkowski diff between the two and find vector from origin to that
            recursively find minkowski point along vector from origin to last point*/
    }

    obj1->hasCollision = collided || obj1->hasCollision;
    obj2->hasCollision = collided || obj2->hasCollision;
}

void GameObject::update(float dt)
{
    glm::vec3 n_Pos, n_Vel, n_Rot, n_RotVel;

    n_Pos = pos + vel * dt + 0.5f * (forces / mass) * dt * dt;
    n_Vel = vel + (forces / mass) * dt;
    n_Rot = rot;
    n_RotVel = rotVel;

    pos = n_Pos;
    vel = n_Vel;
    rot = n_Rot;
    rotVel = n_RotVel;

    m_transform = Transform(pos, rot, scale);
    forces = glm::vec3(0.f); // reset the forces
    hasCollision = false;
}

void GameObject::addForce(glm::vec3 force)
{
    forces += force;
}

glm::vec3 GameObject::getForce()
{
    return forces;
}

glm::vec3 GameObject::getPos()
{
    return m_transform.position();
}

glm::mat4 GameObject::getTransform()
{
    return m_transform.T();
}

glm::vec4 GameObject::getColor()
{
    if(hasCollision)
    {
        return glm::vec4(0,1,0,1);
    }
    else
    {
        return glm::vec4(1,0,0,1);
    }
}
