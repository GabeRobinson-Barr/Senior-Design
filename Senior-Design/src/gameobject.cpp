#include "gameobject.h"
#include "player.h"
#include <iostream>
using namespace std;

GameObject::GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type) :
    pos(pos), rot(rot), scale(scale), m_transform(Transform(pos, rot, scale)),
    vel(glm::vec3(0.f)),  rotVel(glm::vec3(0.f)), mass(mass), drag(1.f), angDrag(1.f),
    forces(glm::vec3(0.f)), torque(glm::vec3(0.f)), geomType(type), isDynamic(true),
    connectedComp(nullptr), maxSpd(1000.f)
{
    //TODO:
    //CREATE COLLIDER
    //create matrices
    isSticky = false;
    if(geomType == SPHERE)
    {
        moment = glm::vec3((2.f/5.f) * mass * pow(scale.x / 2.f,2.f));
        tensor = glm::mat3(1.f);
        //isSticky = true;
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

std::pair<bool,glm::vec3> GameObject::collide(GameObject *obj1, GameObject *obj2)
{
    bool collided = false;
    glm::vec3 collisionPt = glm::vec3(0);

    if(obj1->geomType == SPHERE && obj2->geomType == SPHERE) // simple sphere check
    {
        glm::vec3 diff = glm::abs(obj1->getPos() - obj2->getPos());
        float collisionDist = (obj1->scale.x + obj2->scale.x) / 2.f;
        if(glm::length(diff) <= collisionDist)
        {
            collided = true;
            // find the point of collision and the magnitude of force and apply it to both objects
            collisionPt = obj2->getPos() + (glm::normalize(obj1->getPos() - obj2->getPos()) * obj2->scale * 0.5f);
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
        }

    }
    obj1->hasCollision = collided || obj1->hasCollision;
    obj2->hasCollision = collided || obj2->hasCollision;
    std::pair<bool,glm::vec3> p = std::make_pair(collided,glm::vec3(collisionPt));
    return p;
}

void GameObject::addCollision(GameObject *obj, glm::vec3 collisionPt)
{
    glm::vec3 objNor = obj->getNor(collisionPt - obj->getPos());
    glm::vec3 endVel = ((obj->vel * (obj->mass * 2.f)/(obj->mass + this->mass)) +
                         (this->vel * (this->mass - obj->mass) / (obj->mass + this->mass)));
    glm::vec3 force = glm::length(endVel - this->vel) * glm::normalize(endVel + objNor) * this->mass;
    this->addForce(force, collisionPt);

    glm::vec3 scl = (obj->scale + this->scale) * 0.5f;
    this->translate(scl * 1.001f * glm::length(this->vel) * (16.f / 1000.f) * (objNor));
}

void GameObject::addStickyCollision(GameObject *obj1, GameObject *obj2, glm::vec3 collisionPt)
{
    float mass1 = obj1->mass;
    float mass2 = obj2->mass;

    if(obj1->connectedComp == nullptr && obj2->connectedComp == nullptr)
    {
        ConnectedObject* connect = new ConnectedObject();
        connect->addObj(obj1);
        connect->addObj(obj2);
    }
    else if(obj1->connectedComp != nullptr)
    {
        mass1 = obj1->connectedComp->getMass();
        if(obj2->connectedComp != nullptr)
        {
            mass2 = obj2->connectedComp->getMass();
            if(ConnectedObject::canCollide(obj1, obj2))
            {
                ConnectedObject::mergeConnectedObjs(obj1->connectedComp, obj2->connectedComp);
            }
            else{
                return;
            }
        }
        else
        {
            obj1->connectedComp->addObj(obj2);
        }
    }
    else
    {
        mass2 = obj2->connectedComp->getMass();
        obj2->connectedComp->addObj(obj1);
    }
    glm::vec3 endForce = ((obj1->vel * mass1) + (obj2->vel * mass2));
    collisionPt = (glm::length(obj1->vel * mass1) * obj1->getPos()) - (glm::length(obj2->vel * mass2) * obj2->getPos());
    // Apply the added force to the component
    glm::vec3 force = endForce - (obj1->connectedComp->mass * obj1->connectedComp->vel);
    // using the positions because we are adding them to the connected object and it doesnt rotate otherwise
    obj1->connectedComp->addForce(force, collisionPt);
}

glm::vec3 GameObject::getSupport(glm::vec3 v)
{
    glm::vec3 supVec;
    if (geomType == CUBE)
    {
        v = glm::vec3(glm::inverse(m_transform.rotMat()) * glm::vec4(v,1));
        supVec = glm::vec3(sgn(v.x)*0.5f, sgn(v.y)*0.5f, sgn(v.z)*0.5f);
        if(abs(v.x) < 0.5f) {supVec.x = v.x;}
        if(abs(v.y) < 0.5f) {supVec.y = v.y;}
        if(abs(v.z) < 0.5f) {supVec.z = v.z;}
        supVec = glm::vec3(m_transform.rotMat() * glm::vec4(supVec,1));
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
    if(!isDynamic)
    {
        forces = glm::vec3(0.f);
        torque = glm::vec3(0.f);
        updateTransform();
        updated = true;
        return;
    }
    if(updated)
    {
        return;
    }
    // update through its connected component if possible
    if(connectedComp != nullptr)
    {
        forces = glm::vec3(0.f);
        torque = glm::vec3(0.f);
        connectedComp->update(dt);
        return;
    }
    //cout << "adding force" << endl;
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

    updateTransform();
    forces = glm::vec3(0.f); // reset the forces
    torque = glm::vec3(0.f);
    hasCollision = false;
    updated = true;
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
    if(!isDynamic)
    {
        return;
    }
    if(connectedComp != nullptr)
    {
        if(glm::length(force) >= 10.f)
        {
            connectedComp->removeObj(this);
        }
        else
        {
            connectedComp->addForce(force, collPt);
            return;
        }
    }
    if(geomType == MeshType::SPHERE)
    {
        // apply the force to both objects along the vector from the collision point to their center (for spheres)
        glm::vec3 arm = getPos() - collPt;
        //glm::vec3 norVec = getNor(arm);
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
        //glm::vec3 norVec = getNor(diffVec);
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

glm::vec3 GameObject::getRot()
{
    return rot;
}

glm::vec3 GameObject::getVel()
{
    return vel;
}

glm::vec3 GameObject::getRotVel()
{
    return rotVel;
}

Transform GameObject::getTransform()
{
    return m_transform;
}

glm::vec4 GameObject::getColor()
{
    //if(hasCollision)
    //{
    //    return glm::vec4(0,1,0,1);
    //}
    //else
    //{
        return glm::vec4(1,0,0,1);
    //}
}

glm::vec3 GameObject::getScale()
{
    return scale;
}

void GameObject::translate(glm::vec3 t)
{
    pos += t;
    updateTransform();
}

void GameObject::updateTransform()
{
    m_transform = Transform(pos, rot, scale);
}

void GameObject::updateTransform(glm::vec3 p, glm::vec3 r, glm::vec3 s)
{
    rot = r;
    pos = p;
    scale = s;
    m_transform = Transform(p, r, s);
}

float GameObject::getMass()
{
    return mass;
}

void GameObject::setVel(glm::vec3 v)
{
    vel = v;
}

void GameObject::setRotVel(glm::vec3 rVel)
{
    rotVel = rVel;
}

void GameObject::setDynamic(bool b)
{
    isDynamic = b;
}
