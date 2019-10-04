#include "gameobject.h"
#include <iostream>
using namespace std;

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

// this is a helper function to calculate the closest point on the given simplex to the origin
// as well as simplifying the points used to describe the simplex if necessary
glm::vec3 closestSimplexPt(std::vector<glm::vec3>* W)
{
    glm::vec3 closest;
    int dim = W->size();
    // Find Closest point
    if(dim == 1)
    {
        closest = W->at(0);
    }
    else if (dim == 2)
    {
        glm::vec3 a = W->at(0);
        glm::vec3 b = W->at(1);
        glm::vec3 lineAB = b - a;
        // Calculate and clamp projection point
        float t = glm::dot(-a, lineAB) / glm::dot(lineAB, lineAB);
        if(t < 0.f) {t = 0.f;}
        else if(t > 1.f) {t = 1.f;}

        closest = a + t * lineAB;
    }
    else if (dim >= 3){
        if(dim > 3) { // Erase the furthest point from the origin in the simplex
            int farIdx = 0;
            float farDist = glm::length(W->at(0));
            for(int i = 1; i < dim; i++)
            {
                float thisDist = glm::length(W->at(i));
                if(thisDist > farDist)
                {
                    farDist = thisDist;
                    farIdx = i;
                }
            }
            W->erase(W->begin() + farIdx);
        }
        // Calculate the closest point on a plane
        glm::vec3 a = W->at(0);
        glm::vec3 b = W->at(1);
        glm::vec3 c = W->at(2);
        glm::vec3 lineAB = b - a;
        glm::vec3 lineBC = c - b;
        glm::vec3 planeNor = glm::cross(lineAB, lineBC);
        planeNor = glm::normalize(planeNor);
        float t = glm::abs(glm::dot(planeNor, -a) - glm::length(a));
        closest = -t * planeNor;

    }

    return closest;
}


void GameObject::collide(GameObject *obj1, GameObject *obj2)
{
    bool collided = false;

    /*if(obj1->geomType == SPHERE && obj2->geomType == SPHERE) // simple sphere check
    {
        glm::vec3 diff = obj1->getPos() - obj2->getPos();
        float collisionDist = (obj1->scale.x + obj2->scale.x) / 2.f;
        if(glm::length(diff) <= collisionDist)
        {
            collided = true;
        }
    }*/
    //else
    //{

        /* GJK Algorithm:
           take closest (support) points to other's origin
           get minkowski diff between the two and find vector from origin to that
           recursively find minkowski point along vector from origin to last point*/

        float epsilon = 0.001f; // distance at which we say objects are "close enough" to collide
        glm::vec3 v = glm::vec3(1.f,0.f,0.f);
        std::vector<glm::vec3> W = std::vector<glm::vec3>(); // Constructed simplex
        float u = 0.f; // Lower bound of collision distance
        bool closeEnough = false;

        v = obj1->getSupport(v) - obj2->getSupport(-v); // initialize v as some arbitrary point on the minkowski sum A - B
        float lenV = glm::length(v);

        while(!closeEnough && lenV >= epsilon)
        {
            glm::vec3 w = obj1->getSupport(-v) - obj2->getSupport(v);
            /*glm::vec3 w1 = obj1->getSupport(-v);
            glm::vec3 w2 =  -obj2->getSupport(v);
            cout << "Sup1: " << w1.x << ", " << w1.y << ", " << w1.z << '\n';
            cout << "Sup2: " << w2.x << ", " << w2.y << ", " << w2.z << '\n';
            cout << "W: " << w.x << ", " << w.y << ", " << w.z << '\n';*/

            float delta = glm::dot(v, w) / lenV;
            u = std::max(u, delta);
            if(u > epsilon) break; // if u is positive then these objects do not collide
            closeEnough = (lenV - u) <= epsilon;
            if(!closeEnough)
            {
                W.push_back(w);
                v = closestSimplexPt(&W);
                lenV = glm::length(v);
                //cout << "V: " << v.x << ", " << v.y << ", " << v.z << '\n';
                //cout << "lenv: " << lenV << '\n';
            }
        }
        if(lenV <= epsilon)
        {
            collided = true;
        }
    //}

    obj1->hasCollision = collided || obj1->hasCollision;
    obj2->hasCollision = collided || obj2->hasCollision;
}

glm::vec3 GameObject::getSupport(glm::vec3 v)
{
    glm::vec3 supVec;
    if (geomType == CUBE)
    {
        supVec = glm::vec3(sgn(v.x)*0.5f, sgn(v.y)*0.5f, sgn(v.z)*0.5f);
    }
    else if (geomType == SPHERE)
    {
        float len = glm::length(v);
        if (len <= 0.001f) // if v has no length return a vector of 0 length
        {
            supVec = glm::vec3(0.f);
        }
        else
        {
            glm::vec3 nor = glm::normalize(glm::vec3(v.x, v.y, v.z));
            supVec = glm::vec3(nor.x*0.5f,nor.y*0.5f,nor.z*0.5f);
        }

    }
    else
    {
        supVec = glm::vec3(0);
    }

    glm::mat4 t = m_transform.T();
    supVec = glm::vec3(m_transform.T() * glm::vec4(supVec,1));
    return supVec;
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
