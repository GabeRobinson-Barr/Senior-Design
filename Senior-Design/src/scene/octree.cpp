#include "octree.h"
#include <iostream>
using namespace std;

Octree::Octree(glm::vec3 min, glm::vec3 max) :
    minPoint(min), maxPoint(max), m_Objs(std::vector<GameObject*>())
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
        nodes[xyz] = new Octree(glm::vec3(minX, minY, minZ), glm::vec3(midX, midY, midZ));
        nodes[xyZ] = new Octree(glm::vec3(minX, minY, midZ), glm::vec3(midX, midY, maxZ));
        nodes[xYz] = new Octree(glm::vec3(minX, midY, minZ), glm::vec3(midX, maxY, midZ));
        nodes[xYZ] = new Octree(glm::vec3(minX, midY, midZ), glm::vec3(midX, maxY, maxZ));
        nodes[Xyz] = new Octree(glm::vec3(midX, minY, minZ), glm::vec3(maxX, midY, midZ));
        nodes[XyZ] = new Octree(glm::vec3(midX, minY, midZ), glm::vec3(maxX, midY, maxZ));
        nodes[XYz] = new Octree(glm::vec3(midX, midY, minZ), glm::vec3(maxX, maxY, midZ));
        nodes[XYZ] = new Octree(glm::vec3(midX, midY, midZ), glm::vec3(maxX, maxY, maxZ));
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
        int nodeIdx = 0;
        glm::vec3 objPos = obj->getPos();
        if(objPos.z > midPoint.z) nodeIdx += 1;
        if(objPos.y > midPoint.y) nodeIdx += 2;
        if(objPos.x > midPoint.x) nodeIdx += 4;
        nodes[nodeIdx]->add(obj);
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
