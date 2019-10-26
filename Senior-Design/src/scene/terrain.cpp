#include <scene/terrain.h>

#include <scene/cube.h>
#include <iostream>
using namespace std;

Terrain::Terrain() : root(Octree(-dimensions / 2.f, dimensions / 2.f))
{}


void Terrain::CreateTestScene()
{
    // Create the basic terrain floor
    GameObject* obj1 = new GameObject(glm::vec3(0,1,0), glm::vec3(0,0,0), glm::vec3(4,4,4), 1, MeshType::CUBE);
    GameObject* obj2 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
    //GameObject* obj3 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,45), glm::vec3(3,3,3), 1, MeshType::CUBE);
    root.add(obj1);
    root.add(obj2);
    //root.add(obj3);

    /*for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            for(int k = 0; k < 10; k++)
            {
                GameObject* obj = new GameObject(glm::vec3(i * 10, j * 10, k * 10), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::CUBE);
                root.add(obj);
                if(i == 0)
                {
                    obj->addForce(glm::vec3(j, 0,0) / (16.f / 1000.f));
                }
                if(j == 0)
                {
                    obj->addForce(glm::vec3(0,k,0) / (16.f / 1000.f));
                }
            }
        }
    }*/

    obj1->addForce(2.f, obj1->getPos() + glm::vec3(-1,0,0)); // adding initial force for testing
    obj1->addstuff = true;

}

void Terrain::checkCollisions()
{
    //std::vector<GameObject*> objs = root.getObjects();
    std::vector<std::pair<GameObject*, GameObject*>> objPairs;
    root.getCollisionPairs(objPairs);
    for(int i = 0; i < objPairs.size(); i++)
    {
        GameObject* obj1 = objPairs.at(i).first;
        GameObject* obj2 = objPairs.at(i).second;
        GameObject::collide(obj1, obj2);
    }

}

void Terrain::update(float dt)
{
    for(GameObject* obj : root.getObjects())
    {
        obj->update(dt);
    }
    root.update();

    checkCollisions();
}

std::vector<GameObject*> Terrain::getObjects()
{
    return root.getObjects();
}

