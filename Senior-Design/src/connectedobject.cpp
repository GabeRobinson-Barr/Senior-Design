#include "gameobject.h"
#include "math.h"
#include <iostream>
using namespace std;

ConnectedObject::ConnectedObject() : GameObject(glm::vec3(0), glm::vec3(0), glm::vec3(0.f), 0.f, CUBE),
    objs(std::vector<GameObject*>())
{
}

ConnectedObject::~ConnectedObject()
{

}

void ConnectedObject::update(float dt)
{
    if(!isDynamic)
    {
        for(GameObject* g : objs)
        {
            g->setVel(vel);
            g->setRotVel(rotVel);
            g->updated = true;
            g->updateTransform();
        }
        updateTransform();
        return;
    }
    if(mass == 0.f)
    {
        return;
    }
    glm::vec3 dPos, dVel, dRot, dRotVel;
    dPos = vel * dt + 0.5f * (forces / mass) * dt * dt;
    dVel = (forces / mass) * dt;

    glm::vec3 rotMoment = moment;
    if(geomType == CUBE)
    {
        rotMoment = glm::vec3(m_transform.rotMat() * glm::vec4(moment,1));
    }
    dRot = rotVel * dt + 0.5f * (torque / rotMoment) * dt * dt;
    dRotVel = (torque / rotMoment) * dt;

    pos = dPos + pos;
    vel = dVel + vel;
    rot = dRot + rot;
    rotVel = dRotVel + rotVel;

    Transform newTrans = Transform(pos, rot, scale);

    for(GameObject* g : objs)
    {
        glm::vec3 objPos = g->getPos();
        objPos = glm::vec3(m_transform.invT() * glm::vec4(objPos,1));
        objPos = glm::vec3(newTrans.T() * glm::vec4(objPos,1));
        glm::vec3 objRot = g->getRot() + dRot;
        g->setVel(vel + dVel);
        //cout << "ObjVel: " << g->getVel().x << ", " << g->getVel().y << ", " << g->getVel().z << '\n';
        g->setRotVel(rotVel + dRotVel);
        g->updateTransform(objPos, objRot, g->getScale());
        g->updated = true;
        //g->forces = glm::vec3(0.f);
        //g->torque = glm::vec3(0.f);
    }
    //cout << "ObjVel: " << vel.x << ", " << vel.y << ", " << vel.z << '\n';
    //cout << "End Updating Component" << '\n';

    m_transform = newTrans;
    forces = glm::vec3(0.f); // reset the forces
    torque = glm::vec3(0.f);
    hasCollision = false;
}

void ConnectedObject::addForce(glm::vec3 force, glm::vec3 collPt)
{
    this->GameObject::addForce(force, collPt);
}

bool ConnectedObject::canCollide(GameObject *obj1, GameObject *obj2)
{
    return (obj1->connectedComp != obj2->connectedComp);
}

void ConnectedObject::addObj(GameObject *obj)
{
    objs.push_back(obj);
    if(obj->connectedComp != nullptr)
    {
        // this case should have already been checked when colliding
        return;
    }
    obj->connectedComp = this;
    isDynamic = isDynamic && obj->isDynamic;
    // update the mass and position of this object, rotation stays the same
    mass += obj->getMass();
    pos = ((pos * float(objs.size() - 1)) + obj->getPos()) / float(objs.size());
    // update this component's scales
    glm::vec3 scaleFac = glm::abs(glm::vec3(m_transform.rotMat() * glm::vec4(scale * 0.5f,1)));
    glm::vec3 currMin = getPos() - scaleFac;
    glm::vec3 currMax = getPos() + scaleFac;
    glm::vec3 objPos = obj->getPos();
    glm::vec3 objScaleFac = glm::abs(glm::vec3(obj->getTransform().rotMat() * glm::vec4(obj->getScale() * 0.5f,1)));
    currMin.x = glm::min(currMin.x, objPos.x - objScaleFac.x);
    currMin.y = glm::min(currMin.y, objPos.y - objScaleFac.y);
    currMin.z = glm::min(currMin.z, objPos.z - objScaleFac.z);
    currMax.x = glm::max(currMax.x, objPos.x + objScaleFac.x);
    currMax.y = glm::max(currMax.y, objPos.y + objScaleFac.y);
    currMax.z = glm::max(currMax.z, objPos.z + objScaleFac.z);
    scale = glm::vec3(glm::inverse(m_transform.rotMat()) * glm::vec4(currMax - currMin,1));
    updateTransform();
    updateMoment();
}

void ConnectedObject::removeObj(GameObject *obj)
{
    bool stillDynamic = true;
    for(int i = 0; i < objs.size(); i++)
    {
        if(obj == objs.at(i))
        {
            pos = ((pos * float(objs.size())) - obj->getPos()) / float(objs.size() - 1);
            mass -= obj->getMass();
            objs.erase(objs.begin() + i);
            obj->connectedComp = nullptr;
            // find the scale using the remaining objects
            glm::vec3 minPos = pos;
            glm::vec3 maxPos = pos;
            for(GameObject* g : objs)
            {
                stillDynamic = stillDynamic && g->isDynamic;
                glm::vec3 objPos = obj->getPos();
                glm::vec3 objScaleFac = glm::abs(glm::vec3(obj->getTransform().rotMat() * glm::vec4(obj->getScale() * 0.5f,1)));
                glm::vec3 objMin = objPos - objScaleFac;
                glm::vec3 objMax = objPos + objScaleFac;
                for(int idx = 0; idx < 3; idx++)
                {
                    if(objMin[idx] < minPos[idx])
                    {
                        minPos[idx] = objMin[idx];
                    }
                    if(objMax[idx] > maxPos[idx])
                    {
                        maxPos[idx] = objMax[idx];
                    }
                }
                // rotate the scale into object space
                scale = glm::vec3(glm::inverse(m_transform.rotMat()) * glm::vec4(objMax - objMin,1));
            }
            updateTransform();
            break;
        }
    }
    isDynamic = stillDynamic;
}

ConnectedObject* ConnectedObject::mergeConnectedObjs(ConnectedObject *c1, ConnectedObject *c2)
{
    for(GameObject* g : c2->objs)
    {
        c1->addObj(g);
        g->connectedComp = c1;
    }
    delete c2;
}

void ConnectedObject::updateMoment()
{
    moment = glm::vec3((1.f/12.f) * mass * (pow(scale.y,2.f) + pow(scale.z,2.f)),
                       (1.f/12.f) * mass * (pow(scale.x,2.f) + pow(scale.z,2.f)),
                       (1.f/12.f) * mass * (pow(scale.x,2.f) + pow(scale.y,2.f)));
}
