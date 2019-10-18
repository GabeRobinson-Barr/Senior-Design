#ifndef OCTREE_H
#define OCTREE_H
#include "gameobject.h"

enum OCTREE_NODE { // Enum for getting octtree nodes, lowercase is negative, upper case is positive
    xyz,
    xyZ,
    xYz,
    xYZ,
    Xyz,
    XyZ,
    XYz,
    XYZ
};

class Octree
{
public:
    Octree(glm::vec3 min, glm::vec3 max);
    Octree(glm::vec3 min, glm::vec3 max, Octree* parent);
    std::vector<GameObject*> m_Objs; // Vector of gameobjects within this node

    Octree* getNode(OCTREE_NODE node);

    void update(); // reassigns all objects in the scene
    void clear(); // clears all objects from the octree
    void remove(GameObject* obj, glm::vec3 oldPos); // using oldpos to find the nodes it is currently in before moving
    bool move(GameObject* obj, glm::vec3 oldPos); // returns true when a node no longer contains the object
    void add(GameObject* obj);
    bool contains(glm::vec3 minPos, glm::vec3 maxPos); // helper function that returns whether or not the bounds overlap this node
    std::vector<std::pair<GameObject*, GameObject*>>& getCollisionPairs(std::vector<std::pair<GameObject*,GameObject*>>& collisionPairs);

    std::vector<GameObject*> getObjects();

    glm::vec3 minPoint; // these two vec3s give the bounds of a node
    glm::vec3 midPoint;
    glm::vec3 maxPoint;

private:
    Octree* parent;
    Octree* nodes[8];

    bool isLeaf;

};

#endif // OCTTREE_H
