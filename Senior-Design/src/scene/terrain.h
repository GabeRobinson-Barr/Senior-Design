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
    void update(float dt);
    void checkCollisions();

    std::vector<GameObject*> getObjects();

    const glm::vec3 dimensions = glm::vec3(500,500,500);


private:
    Octree root; // root of the octree holding all world elements
    std::vector<GameObject*> gameObjs; // all objects in the world

};
