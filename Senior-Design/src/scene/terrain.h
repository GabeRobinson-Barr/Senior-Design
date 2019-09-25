#pragma once
#include <QList>
#include <la.h>
#include "gameobject.h"
#include "octree.h"

class Terrain
{
public:
    Terrain();

    void CreateTestScene();
    void updateCollisions();

    std::vector<GameObject*> getObjects();

    const glm::vec3 dimensions = glm::vec3(80,80,80);


private:
    Octree root; // root of the octree holding all world elements
    std::vector<GameObject*> gameObjs; // all objects in the world

};
