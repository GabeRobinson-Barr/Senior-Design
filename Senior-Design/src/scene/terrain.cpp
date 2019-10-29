#include <scene/terrain.h>

#include <scene/cube.h>
#include <iostream>
using namespace std;

Terrain::Terrain() : root(Octree(-dimensions / 2.f, dimensions / 2.f))
{}


void Terrain::CreateTestScene()
{
    // Create the basic terrain floor
    //GameObject* obj1 = new GameObject(glm::vec3(0,1,0), glm::vec3(0,0,60), glm::vec3(1,1,1), 1, MeshType::CUBE);
    //GameObject* obj2 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::CUBE);
    //GameObject* obj3 = new GameObject(glm::vec3(20,0,0), glm::vec3(0,0,45), glm::vec3(3,3,3), 3, MeshType::SPHERE);
    //root.add(obj1);
    //root.add(obj2);
    //root.add(obj3);
    int s = 0;
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            for(int k = 0; k < 10; k++)
            {
                GameObject* obj;
                if(s==0)
                {
                    s = 1;
                    obj = new GameObject(glm::vec3(i * 10 + (float)j*(float)k/100.f, j * 10, k * 10), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::CUBE);
                }
                else
                {
                    s = 0;
                    obj = new GameObject(glm::vec3(i * 10 + (float)j*(float)k/100.f, j * 10, k * 10), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
                }
                root.add(obj);
                if(i == 0)
                {
                    obj->addForce(glm::vec3((float)j/5.f, 0,0), obj->getPos() + glm::vec3(-1,0,0));
                }
                if(j == 0)
                {
                    obj->addForce(glm::vec3(0,(float)k/5.f,0), obj->getPos() + glm::vec3(0, -1,0));
                }
            }
        }
    }

    //obj1->addForce(glm::vec3(2.f,0,0), obj1->getPos() + glm::vec3(-1,-1,0)); // adding initial force for testing
    //obj3->addForce(glm::vec3(-12.f,0,0), obj3->getPos() + glm::vec3(1,0,0));

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

