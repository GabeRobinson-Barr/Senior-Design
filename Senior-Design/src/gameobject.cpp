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
/*glm::vec3 closestSimplexPt(std::vector<glm::vec3>* W)
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
        float minDist = glm::length(W->at(0));
        closest = W->at(0);
        for(int i = 0; i < dim; i++)
        {
            glm::vec3 a = W->at(i);
            glm::vec3 b = W->at((i + 1) % dim);
            glm::vec3 c = W->at((i + 2) % dim);
            glm::vec3 lineAB = b - a;
            glm::vec3 lineBC = c - b;
            glm::vec3 planeNor = glm::cross(lineAB,lineBC);
            planeNor = glm::normalize(planeNor);
            float t = glm::abs(glm::dot(planeNor, -a) - glm::length(a));
            if(t < minDist){
                minDist = t;
                closest = -t * planeNor;
            }

        }
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
}*/


void GameObject::collide(GameObject *obj1, GameObject *obj2)
{
    bool collided = false;
    glm::vec3 collisionPt;

    if(obj1->geomType == SPHERE && obj2->geomType == SPHERE) // simple sphere check
    {
        glm::vec3 diff = glm::abs(obj1->getPos() - obj2->getPos());
        float collisionDist = (obj1->scale.x + obj2->scale.x) / 2.f;
        if(glm::length(diff) <= collisionDist)
        {
            collided = true;
            // find the point of collision and the magnitude of force and apply it to both objects
            collisionPt = obj2->getPos() + (glm::normalize(obj1->getPos() - obj2->getPos()) * obj2->scale * 0.5f);
            float force = glm::length((obj2->vel * obj2->mass) + (obj1->vel * obj1->mass));
            obj1->addForce(force, collisionPt);
            obj2->addForce(force, collisionPt);
            // Move the objects away from each other by the difference in their distance to the collision distance
            // so that they dont collide on multiple frames
            obj1->translate((collisionDist - glm::length(diff)) * glm::normalize(obj1->getPos() - collisionPt));
            obj2->translate((collisionDist - glm::length(diff)) * glm::normalize(obj2->getPos() - collisionPt));
        }
    }
    else if (obj1->geomType == CUBE && obj2->geomType == CUBE)
    {
        //std::vector<int> collisionIdx = std::vector<int>();
        glm::vec3 collisionDist;
        for (int i = 0; i < 8; i++)
        {
            int idx = i;
            glm::vec3 corner = glm::vec3(-0.5f, -0.5f, -0.5f);
            if(idx >= 4)
            {
                corner.x = 0.5f;
                idx -= 4;
            }
            if(idx >= 2)
            {
                corner.y = 0.5f;
                idx -= 2;
            }
            if(idx >= 1)
            {
                corner.z = 0.5f;
            }
            glm::vec3 cub1Pt = corner;
            cub1Pt = glm::vec3(obj1->m_transform.T() * glm::vec4(cub1Pt,1)); // world space corner point on cube1

            glm::vec3 cub2Pt = corner;
            cub2Pt = glm::vec3(obj2->m_transform.T() * glm::vec4(cub2Pt,1)); // world space corner point on cube2

            // Move the support points into each other's space
            cub1Pt = glm::vec3(obj2->m_transform.invT() * glm::vec4(cub1Pt,1));
            cub2Pt = glm::vec3(obj1->m_transform.invT() * glm::vec4(cub2Pt,1));

            glm::vec3 cub1PtAbs = glm::abs(cub1Pt);
            glm::vec3 cub2PtAbs = glm::abs(cub2Pt);

            glm::vec3 scale1 = glm::vec3(0.5f,0.5f,0.5f);
            glm::vec3 scale2 = glm::vec3(0.5f,0.5f,0.5f);

            collisionDist = glm::vec3(0);
            if(cub1PtAbs.x <= scale2.x && cub1PtAbs.y <= scale2.y && cub1PtAbs.z <= scale2.z)
            {
                collided = true;
                collisionPt = glm::vec3(obj2->m_transform.T() * glm::vec4(cub1Pt,1)); // move the collision point back into world space
                collisionDist = scale2 - cub1Pt;
                break;
            }
            else if(cub2PtAbs.x <= scale1.x && cub2PtAbs.y <= scale1.y && cub2PtAbs.z <= scale1.z)
            {
                collided = true;
                collisionPt = glm::vec3(obj1->m_transform.T() * glm::vec4(cub2Pt,1));
                collisionDist = scale1 - cub2Pt;
                break;
            }
        }
        if(collided)
        {
            float force = glm::length((obj2->vel * obj2->mass) + (obj1->vel * obj1->mass));
            obj1->addForce(force, collisionPt);
            obj2->addForce(force, collisionPt);
            // Move the objects away from each other by the difference in their distance to the collision distance
            // so that they dont collide on multiple frames
            obj1->translate((collisionDist) * glm::normalize(obj1->getPos() - collisionPt));
            obj2->translate((collisionDist) * glm::normalize(obj2->getPos() - collisionPt));
        }

    }
    else
    {
        GameObject* cube;
        GameObject* sph;
        if(obj1->geomType == SPHERE)
        {
            sph = obj1;
            cube = obj2;
        }
        else
        {
            sph = obj2;
            cube = obj1;
        }
        glm::vec3 cubSpaceSph = glm::vec3(cube->m_transform.invT() * glm::vec4(sph->getPos(),1));
        glm::vec3 cubPoint = cube->getSupport(cubSpaceSph);
        glm::vec3 dist = glm::abs(cubSpaceSph - cubPoint);
        if(glm::length(dist) <= sph->scale.x * 0.5f)
        {
            collided = true;
            collisionPt = glm::vec3(cube->m_transform.T() * glm::vec4(cubPoint,1));
            glm::vec3 collisionNor = glm::normalize(cube->getPos() - sph->getPos());

        }

    }

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

    //glm::mat4 t = m_transform.T();
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

void GameObject::addForce(float force, glm::vec3 collPt)
{
    if(geomType == MeshType::SPHERE)
    {
        // apply the force to both objects along the vector from the collision point to their center (for spheres)
        glm::vec3 forceVec = (force * glm::normalize(getPos() - collPt)) / (mass * 16.f / 1000.f);
        forces += forceVec;
    }
    else if(geomType == MeshType::CUBE)// && addstuff)
    {
        glm::vec3 diffVec = glm::normalize(getPos() - collPt);
        glm::vec3 norVec = glm::vec3(1.f * sgn(diffVec.x),0,0);
        float maxComp = glm::abs(diffVec.x);
        for(int i = 1; i < 3; i++)
        {
            if(glm::abs(diffVec[i] - maxComp) <= 0.0001f) // if 2 components are equal (ignoring floating pt error)
            {
                norVec[i] = 1.f * sgn(diffVec[i]);
            }
            else if(glm::abs(diffVec[i]) > maxComp)
            {
                norVec = glm::vec3(0);
                norVec[i] = 1.f * sgn(diffVec[i]);
                maxComp = glm::abs(diffVec[i]);
            }
        }
        norVec = glm::normalize(norVec);
        glm::vec3 forceVec = (force * norVec) / (mass * 16.f / 1000.f);
        forces += forceVec;
        addstuff = false;
    }
    else {
        // UNSUPPORTED SHAPES
    }


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

glm::vec3 GameObject::getScale()
{
    return scale;
}

void GameObject::translate(glm::vec3 t)
{
    pos += t;
    m_transform = Transform(pos, rot, scale);
}

