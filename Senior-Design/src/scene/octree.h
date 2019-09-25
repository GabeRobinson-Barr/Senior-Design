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
    std::vector<GameObject*> m_Objs; // Vector of gameobjects within this node

    Octree* getNode(OCTREE_NODE node);

    void remove(GameObject* obj);
    void move(GameObject* obj);
    void add(GameObject* obj);

    std::vector<GameObject*> getObjects();

    glm::vec3 minPoint; // these two vec3s give the bounds of a node
    glm::vec3 midPoint;
    glm::vec3 maxPoint;

private:
    Octree* nodes[8];

    bool isLeaf;

};

#endif // OCTTREE_H
