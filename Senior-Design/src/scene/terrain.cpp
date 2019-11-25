#include <scene/terrain.h>

#include <scene/cube.h>
#include <iostream>
using namespace std;

Terrain::Terrain() : root(Octree(-dimensions, dimensions))
{}


void Terrain::CreateTestScene()
{
    setupLevel();
    // Create the basic terrain floor
    //GameObject* obj1 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(10,10,10), 1, MeshType::CUBE);
    //GameObject* obj2 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
    //GameObject* obj3 = new GameObject(glm::vec3(20,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 3, MeshType::SPHERE);
    //root.add(obj1);
    //obj1->setDynamic(false);
    //root.add(obj2);
    //root.add(obj3);
    /*int s = 0;
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
                    obj = new GameObject(glm::vec3(i * 10 + (float)j*(float)k/100.f, j * 10, k * 10), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
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
    }*/

    //obj1->addForce(glm::vec3(0,2.f * obj1->getMass(),0), obj1->getPos() + glm::vec3(0,-1,0)); // adding initial force for testing
    //obj3->addForce(glm::vec3(-3.f,0,0), obj3->getPos() + glm::vec3(1,0,0));

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
        // Only check collisions if these objects are not in the same component
        if(obj1->connectedComp == nullptr || obj2->connectedComp == nullptr ||
                ConnectedObject::canCollide(obj1, obj2))
        {
            std::pair<bool,glm::vec3> pair = GameObject::collide(obj1, obj2);
            if(pair.first)
            {
                //cout << "collided" << endl;
                if(obj1->isSticky || obj2->isSticky)
                {
                    GameObject::addStickyCollision(obj1, obj2, pair.second);
                }
                else
                {
                    if(!obj1->noColl)
                    {
                        obj1->addCollision(obj2,pair.second);
                    }
                    else
                    {
                        obj1->noColl = false;
                    }
                    if(!obj2->noColl)
                    {
                        obj2->addCollision(obj1,pair.second);
                    }
                    else
                    {
                        obj2->noColl = false;
                    }

                }

            }
        }
    }

}

void Terrain::update(float dt)
{
    for(GameObject* obj : root.getObjects())
    {
        obj->updated = false;
    }
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

void Terrain::addPlayer(Player *p)
{
    root.add(p);
}

void Terrain::setupLevel()
{
    GameObject* obj = new GameObject(glm::vec3(0,-0.5f,0), glm::vec3(0.f), glm::vec3(dimensions.x / 2.f, 1.f, dimensions.z / 2.f), 1.f, CUBE);
    obj->isDynamic = false;
    root.add(obj);
    /*obj = new GameObject(glm::vec3(0,250.f,0), glm::vec3(0.f), glm::vec3(dimensions.x / 2.f, 1.f, dimensions.z / 2.f), 1.f, CUBE);
    obj->isDynamic = false;
    root.add(obj);
    obj = new GameObject(glm::vec3(60.f,3.f,3.f), glm::vec3(0.f), glm::vec3(5.f), 1.f, SPHERE);
    root.add(obj);
    obj->addForce(glm::vec3(-8.f,1.f,0),obj->getPos() + glm::vec3(-1.f,-0.1f,0));
    obj = new GameObject(glm::vec3(0,10.f,0), glm::vec3(0.f), glm::vec3(10.f), 1.f, CUBE);
    obj->isDynamic = false;
    root.add(obj);*/
}
