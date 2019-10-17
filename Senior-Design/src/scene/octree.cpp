#include "octree.h"
#include <iostream>
using namespace std;

Octree::Octree(glm::vec3 min, glm::vec3 max) : Octree(min, max, nullptr)
{
}

Octree::Octree(glm::vec3 min, glm::vec3 max, Octree* parent) :
    minPoint(min), maxPoint(max), m_Objs(std::vector<GameObject*>()), parent(parent)
{
    float minX, minY, minZ, maxX, maxY, maxZ;
    float midX, midY, midZ, range;
    minX = minPoint.x;
    minY = minPoint.y;
    minZ = minPoint.z;
    maxX = maxPoint.x;
    maxY = maxPoint.y;
    maxZ = maxPoint.z;
    midX = ((maxX - minX) / 2.0f) + minX;
    midY = ((maxY - minY) / 2.0f) + minY;
    midZ = ((maxZ - minZ) / 2.0f) + minZ;
    range = maxX - minX; // since each node should be a square range is equal in all directions
    midPoint = glm::vec3(midX, midY, midZ);
    // Setup the 8 children as empty nodes
    if (range > 10)
    {
        //cout << minX << "," << maxX << '\n';
        nodes[xyz] = new Octree(glm::vec3(minX, minY, minZ), glm::vec3(midX, midY, midZ), this);
        nodes[xyZ] = new Octree(glm::vec3(minX, minY, midZ), glm::vec3(midX, midY, maxZ), this);
        nodes[xYz] = new Octree(glm::vec3(minX, midY, minZ), glm::vec3(midX, maxY, midZ), this);
        nodes[xYZ] = new Octree(glm::vec3(minX, midY, midZ), glm::vec3(midX, maxY, maxZ), this);
        nodes[Xyz] = new Octree(glm::vec3(midX, minY, minZ), glm::vec3(maxX, midY, midZ), this);
        nodes[XyZ] = new Octree(glm::vec3(midX, minY, midZ), glm::vec3(maxX, midY, maxZ), this);
        nodes[XYz] = new Octree(glm::vec3(midX, midY, minZ), glm::vec3(maxX, maxY, midZ), this);
        nodes[XYZ] = new Octree(glm::vec3(midX, midY, midZ), glm::vec3(maxX, maxY, maxZ), this);
        isLeaf = false;
    }
    else
    {
        for(int i = 0; i < 8; i++)
        {
            nodes[i] = nullptr;
        }
        isLeaf = true;
    }
}

void Octree::add(GameObject *obj)
{
    m_Objs.push_back(obj);
    if(!isLeaf)
    {
        glm::vec3 objPos = obj->getPos();
        glm::vec3 objMax = objPos + (obj->getScale() / 2.f);
        glm::vec3 objMin = objPos - (obj->getScale() / 2.f);

        // Place objects into any overlapping nodes
        for(int idx = 0; idx < 8; idx++)
        {
            glm::vec3 nodeMax = nodes[idx]->maxPoint;
            glm::vec3 nodeMin = nodes[idx]->minPoint;
            // If either the min or max of all 3 dimensions falls within the node place this in the node
            if(((objMin.x >= nodeMin.x && objMin.x <= nodeMax.x) || (objMax.x >= nodeMin.x && objMax.x <= nodeMax.x))
                && ((objMin.y >= nodeMin.y && objMin.y <= nodeMax.y) || (objMax.y >= nodeMin.y && objMax.y <= nodeMax.y))
                && ((objMin.z >= nodeMin.z && objMin.z <= nodeMax.z) || (objMax.z >= nodeMin.z && objMax.z <= nodeMax.z)))
            {
                nodes[idx]->add(obj);
            }
        }
    }
}

void Octree::move(GameObject *obj)
{
    // TODO IMPLEMENT
    // remove object and add it again?
}

void Octree::remove(GameObject *obj)
{

}

Octree* Octree::getNode(OCTREE_NODE node)
{
    return nodes[node];
}

std::vector<GameObject*> Octree::getObjects()
{
    return m_Objs;
}

std::vector<std::pair<GameObject*,GameObject*>>& Octree::getCollisionPairs(std::vector<std::pair<GameObject*,GameObject*>>& collisionPairs)
{
    // This node is guarenteed to have at least 2 objects in it
    if(isLeaf) // If this is a leaf, check dist and return possible collision pairs
    {
        for(int i = 0; i < m_Objs.size(); i++)
        {
            for(int j = i; j < m_Objs.size(); j++)
            {
                GameObject* obj1 = m_Objs.at(i);
                GameObject* obj2 = m_Objs.at(j);
                float dist = glm::length(obj1->getPos() - obj2->getPos());
                glm::vec3 scaledist = obj1->getScale() + obj2->getScale();
                float mindist = glm::max(scaledist.x, glm::max(scaledist.y, scaledist.z));
                if(dist <= mindist) // if the objects are closer than their largest scale factor combined (and doubled) then check for a collision
                {
                    collisionPairs.insert(collisionPairs.end(), std::make_pair<GameObject*,GameObject*>((GameObject*)obj1, (GameObject*)obj2));
                }
            }
        }
    }
    else // this is not a leaf, recurse on children nodes if they have possible collisions (2 or more objects)
    {
        for(int i = 0; i < 8; i++)
        {
            if(nodes[i]->m_Objs.size() >= 2)
            {
                nodes[i]->getCollisionPairs(collisionPairs);
            }
        }
    }


    return collisionPairs;
}
