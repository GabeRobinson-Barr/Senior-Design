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
    if (range > 40)
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

void Octree::update()
{
    std::vector<GameObject*> sceneObjs = m_Objs;
    clear();
    for(GameObject* obj : sceneObjs)
    {
        add(obj);
    }
}

void Octree::clear()
{
    m_Objs.clear();
    if(!isLeaf){
        for(int i = 0; i < 8; i++)
        {
            nodes[i]->clear();
        }
    }
}

void Octree::add(GameObject *obj)
{
    m_Objs.push_back(obj);
    if(!isLeaf)
    {
        glm::vec3 objPos = obj->getPos();
        glm::vec3 objMax = objPos + obj->getScale();
        glm::vec3 objMin = objPos - obj->getScale();
        // this will give a buffer to place objects in nodes before they arrive in the actual node
        // that scales with the object scale

        // Place objects into any overlapping nodes
        for(int idx = 0; idx < 8; idx++)
        {
            // If either the min or max of all 3 dimensions falls within the node place this in the node
            if(nodes[idx]->contains(objMin, objMax))
            {
                nodes[idx]->add(obj);
            }
        }
    }
}

bool Octree::move(GameObject *obj, glm::vec3 oldPos)
{
    glm::vec3 objPos = obj->getPos();
    glm::vec3 objMax = objPos + obj->getScale();
    glm::vec3 objMin = objPos - obj->getScale();
    // If this object is still in this node, check child nodes for it
    if(contains(objMin, objMax))
    {
        if(!isLeaf) // If this is not a leaf then find the child nodes containing this object
        {
            objPos = oldPos; // use the old position to find the object in the tree
            objMax = objPos + obj->getScale();
            objMin = objPos - obj->getScale();

            // Place objects into any overlapping nodes
            for(int idx = 0; idx < 8; idx++)
            {
                // If a child node contains the object check if we need to move it
                if(nodes[idx]->contains(objMin, objMax))
                {
                    if(nodes[idx]->move(obj, oldPos)) // if this object is no longer in one of the child nodes
                    {
                        remove(obj, oldPos);
                        add(obj);
                        return false;
                    }
                }
            }
        }
        return false;
    }
    // if this object is no longer in this node, remove it from and re-add it to the parent node
    // we remove this from the parent node because we dont want a duplicate when we re-add it
    else {
        if(parent != nullptr)
        {
            cout << "Moved from " << minPoint.x << ", " << minPoint.y << ", " << minPoint.z;
            cout << " - " << maxPoint.x << ", " << maxPoint.y << ", " << maxPoint.z << '\n';
            cout << "Bounds " << objMin.x << ", " << objMin.y << ", " << objMin.z;
            cout << " - " << objMax.x << ", " << objMax.y << ", " << objMax.z << '\n';
            cout << '\n';
            return true;
        }
        else { // if this object is no longer in the root node delete it from the game
            remove(obj, oldPos);
            delete(obj);
            return true;
        }
    }
}

void Octree::remove(GameObject *obj, glm::vec3 oldPos)
{
    int idx = 0;
    // Find the object in the object vector and erase it
    for(GameObject* currObj : m_Objs)
    {
        if(currObj == obj)
        {
            m_Objs.erase(m_Objs.begin() + idx);
            break;
        }
        idx++;
    }
    if(!isLeaf) // if this isn't a leaf find the nodes containing the object and remove it
    {
        glm::vec3 objPos = oldPos; // use the old position to find the object in the tree
        glm::vec3 objMax = objPos + obj->getScale();
        glm::vec3 objMin = objPos - obj->getScale();

        // Place objects into any overlapping nodes
        for(int idx = 0; idx < 8; idx++)
        {
            // If a child node contains this object remove it from the child
            if(nodes[idx]->contains(objMin, objMax))
            {
                nodes[idx]->remove(obj, oldPos);
            }
        }
    }
}

bool Octree::contains(glm::vec3 minPos, glm::vec3 maxPos)
{
    // If either the min or max of all 3 dimensions are contained in this node then it is in the node
    if(((minPos.x >= minPoint.x && minPos.x <= maxPoint.x) || (maxPos.x >= minPoint.x && maxPos.x <= maxPoint.x))
        && ((minPos.y >= minPoint.y && minPos.y <= maxPoint.y) || (maxPos.y >= minPoint.y && maxPos.y <= maxPoint.y))
        && ((minPos.z >= minPoint.z && minPos.z <= maxPoint.z) || (maxPos.z >= minPoint.z && maxPos.z <= maxPoint.z)))
    {
        return true;
    }
    else
    {
        return false;
    }
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
            for(int j = i + 1; j < m_Objs.size(); j++)
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
