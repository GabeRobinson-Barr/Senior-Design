#include "gameobject.h"
#include <iostream>
using namespace std;

GameObject::GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type) :
    pos(pos), rot(rot), scale(scale), m_transform(Transform(pos, rot, scale)),
    vel(glm::vec3(0.f)),  rotVel(glm::vec3(0.f)), mass(mass), drag(1.f), angDrag(1.f),
    forces(glm::vec3(0.f)), torque(glm::vec3(0.f)), geomType(type)
{
    //TODO:
    //CREATE COLLIDER
    //create matrices
    if(geomType == SPHERE)
    {
        moment = glm::vec3((2.f/5.f) * mass * pow(scale.x / 2.f,2.f));
        tensor = glm::mat3(1.f);
    }
    else if (geomType == CUBE)
    {
        moment = glm::vec3((1.f/12.f) * mass * (pow(scale.y,2.f) + pow(scale.z,2.f)),
                           (1.f/12.f) * mass * (pow(scale.x,2.f) + pow(scale.z,2.f)),
                           (1.f/12.f) * mass * (pow(scale.x,2.f) + pow(scale.y,2.f)));
        tensor = glm::mat3(pow(scale.y,2.f) + pow(scale.z,2.f), -scale.x * scale.y, -scale.x * scale.z,
                           -scale.y * scale.x, pow(scale.x,2.f) + pow(scale.z,2.f), -scale.y * scale.z,
                           -scale.z * scale.x, -scale.z * scale.y, pow(scale.x,2.f) + pow(scale.y,2.f));
    }
}

GameObject::~GameObject()
{
}

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
            glm::vec3 obj1Nor = obj1->getNor(collisionPt - obj1->getPos());
            glm::vec3 obj2Nor = obj2->getNor(collisionPt - obj2->getPos());
            glm::vec3 endVel1 = ((obj1->vel * (obj1->mass - obj2->mass)/(obj1->mass + obj2->mass)) +
                    (obj2->vel * (obj2->mass * 2.f) / (obj1->mass + obj2->mass))) * obj2Nor;
            glm::vec3 endVel2 = ((obj1->vel * (obj1->mass * 2.f)/(obj1->mass + obj2->mass)) +
                                 (obj2->vel * (obj2->mass - obj1->mass) / (obj1->mass + obj2->mass))) * obj1Nor;
            glm::vec3 force1 = endVel1 - obj1->vel;
            glm::vec3 force2 = endVel2 - obj2->vel;
            obj1->addForce(force1, collisionPt);
            obj2->addForce(force2, collisionPt);
            // Move the objects away from each other by the difference in their distance to the collision distance
            // so that they dont collide on multiple frames
            glm::vec3 scl = (obj1->scale + obj2->scale) * 0.5f;
            obj1->translate(scl * 1.1f * (collisionDist - glm::length(diff)) * obj2Nor);
            obj2->translate(scl * 1.1f * (collisionDist - glm::length(diff)) * obj1Nor);
        }
    }
    else if (obj1->geomType == CUBE && obj2->geomType == CUBE)
    {
        std::vector<glm::vec3> collisionPts = std::vector<glm::vec3>();
        std::vector<glm::vec3> collisionPts2 = std::vector<glm::vec3>();
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

            if(cub1PtAbs.x <= scale2.x && cub1PtAbs.y <= scale2.y && cub1PtAbs.z <= scale2.z)
            {
                collided = true;
                collisionPts.push_back(glm::vec3(obj2->m_transform.T() * glm::vec4(cub1Pt,1))); // move the collision point back into world space
            }
            if(cub2PtAbs.x <= scale1.x && cub2PtAbs.y <= scale1.y && cub2PtAbs.z <= scale1.z)
            {
                collided = true;
                collisionPts.push_back(glm::vec3(obj1->m_transform.T() * glm::vec4(cub2Pt,1)));
            }
        }
        if(collided)
        {
            for(glm::vec3 v : collisionPts)
            {
                collisionPt += v;
            }
            collisionPt = collisionPt / (float)collisionPts.size();
            glm::vec3 obj1Nor = obj1->getNor(obj1->getPos() - collisionPt);
            glm::vec3 obj2Nor = obj2->getNor(obj2->getPos() - collisionPt);
            glm::vec3 endVel1 = ((obj1->vel * (obj1->mass - obj2->mass)/(obj1->mass + obj2->mass)) +
                    (obj2->vel * (obj2->mass * 2.f) / (obj1->mass + obj2->mass)));
            glm::vec3 endVel2 = ((obj1->vel * (obj1->mass * 2.f)/(obj1->mass + obj2->mass)) +
                                 (obj2->vel * (obj2->mass - obj1->mass) / (obj1->mass + obj2->mass)));
            glm::vec3 force1 = glm::length(endVel1 - obj1->vel) * obj2Nor;
            glm::vec3 force2 = glm::length(endVel2 - obj2->vel) * obj1Nor;
            obj1->addForce(force1, collisionPt);
            obj2->addForce(force2, collisionPt);

            // Move the objects away from each other by the difference in their distance to the collision distance
            // so that they dont collide on multiple frame
            float collisionDist = glm::length(obj1Nor * ((obj1->scale * 0.5f) - glm::abs(obj1->getPos() - collisionPt))) +
                    glm::length(obj2Nor * ((obj2->scale * 0.5f) - glm::abs(obj2->getPos() - collisionPt)));
            glm::vec3 scl = (obj1->scale + obj2->scale) * 0.5f;
            obj1->translate(scl * 1.1f * (collisionDist) * (obj1->getPos() - obj2->getPos()));
            obj2->translate(scl * 1.1f * (collisionDist) * (obj2->getPos() - obj1->getPos()));
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
        glm::vec3 distVec = glm::abs(cubSpaceSph - cubPoint);
        float dist = glm::length(distVec * cube->scale);

        if(dist <= sph->scale.x * 0.5f) // divide by 2 twice once for cube and once for sph scale
        {
            collided = true;
            collisionPt = glm::vec3(cube->m_transform.T() * glm::vec4(cubPoint,1));
            glm::vec3 obj1Nor = obj1->getNor(obj1->getPos() - collisionPt);
            glm::vec3 obj2Nor = obj2->getNor(obj2->getPos() - collisionPt);
            glm::vec3 force1 = glm::length(obj2->vel * obj2->mass) * obj2Nor;
            glm::vec3 force2 = glm::length(obj1->vel * obj1->mass) * obj1Nor;
            obj1->addForce(force1, collisionPt);
            obj2->addForce(force2, collisionPt);

            float diff = (sph->scale.x * 0.5f) - dist;
            glm::vec3 scl = (sph->scale + cube->scale) * 0.5f;
            obj1->translate(scl * 1.1f * diff * obj2Nor);
            obj2->translate(scl * 1.1f * diff * obj1Nor);
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
        if(abs(v.x) < 0.5f) {supVec.x = v.x;}
        if(abs(v.y) < 0.5f) {supVec.y = v.y;}
        if(abs(v.z) < 0.5f) {supVec.z = v.z;}
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

    if(hasCollision)
    {
        //cout << "position" << pos.x << "," << pos.y << "," << pos.z << '\n';
    }
    n_Pos = pos + vel * dt + 0.5f * (forces / mass) * dt * dt;
    n_Vel = vel + (forces / mass) * dt;

    glm::vec3 rotMoment = moment;
    if(geomType == CUBE)
    {
        rotMoment = glm::vec3(m_transform.rotMat() * glm::vec4(moment,1));
    }
    n_Rot = rot + rotVel * dt + 0.5f * (torque / rotMoment) * dt * dt;
    n_RotVel = rotVel + (torque / rotMoment) * dt;


    pos = n_Pos;
    vel = n_Vel;
    rot = n_Rot;
    rotVel = n_RotVel;

    m_transform = Transform(pos, rot, scale);
    forces = glm::vec3(0.f); // reset the forces
    torque = glm::vec3(0.f);
    hasCollision = false;
}

glm::vec3 GameObject::getNor(glm::vec3 vec)
{
    if(geomType == CUBE)
    {
        glm::vec3 norVec = glm::vec3(glm::inverse(m_transform.rotMat()) * glm::vec4(vec,1));
        norVec = glm::vec3(1.f * sgn(norVec.x),0,0);
        float maxComp = glm::abs(vec.x);
        for(int i = 1; i < 3; i++)
        {
            if(glm::abs(glm::abs(vec[i]) - maxComp) <= 0.0001f) // if 2 components are equal (ignoring floating pt error)
            {
                norVec[i] = 1.f * sgn(vec[i]);
            }
            else if(glm::abs(vec[i]) > maxComp)
            {
                norVec = glm::vec3(0);
                norVec[i] = 1.f * sgn(vec[i]);
                maxComp = glm::abs(vec[i]);
            }
        }
        norVec = glm::vec3(m_transform.rotMat() * glm::vec4(norVec,1));
        return glm::normalize(norVec);
    }
    else {
        return glm::normalize(vec);
    }
}

void GameObject::addForce(glm::vec3 force, glm::vec3 collPt)
{
    cout << "collided" << '\n';
    if(geomType == MeshType::SPHERE)
    {
        // apply the force to both objects along the vector from the collision point to their center (for spheres)
        glm::vec3 arm = getPos() - collPt;
        glm::vec3 norVec = getNor(arm);
        //glm::vec3 forceVec = (glm::length(force) * norVec) / (16.f / 1000.f);
        glm::vec3 forceVec = force / (16.f / 1000.f);
        forces += forceVec;//forceVec;
        torque = glm::cross(force, arm) / (16.f / 1000.f);
        //cout << "sphforce: " << forceVec.x << "," << forceVec.y << "," << forceVec.z << '\n';
    }
    else if(geomType == MeshType::CUBE)// && addstuff)
    {
        glm::vec3 arm = getPos() - collPt;
        glm::vec3 diffVec = glm::normalize(arm);
        glm::vec3 norVec = getNor(diffVec);
        //glm::vec3 forceVec = (glm::length(force) * norVec) / (16.f / 1000.f);
        glm::vec3 forceVec = force / (16.f / 1000.f);
        forces += forceVec;//forceVec;
        addstuff = false;
        torque = glm::cross(force, arm) / (16.f / 1000.f);
        //cout << "cubeforce: " << forceVec.x << "," << forceVec.y << "," << forceVec.z << '\n';
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

