#include <scene/terrain.h>

#include <scene/cube.h>

Terrain::Terrain() : root(Octree(-dimensions / 2.f, dimensions / 2.f))
{}


void Terrain::CreateTestScene()
{
    // Create the basic terrain floor
    GameObject* obj1 = new GameObject(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
    GameObject* obj2 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(3,3,3), 1, MeshType::SPHERE);
    root.add(obj1);
    root.add(obj2);

    obj1->addForce(glm::vec3(2.0f, 0,0) / (16.f / 1000.f)); // adding initial force for testing

}

void Terrain::checkCollisions()
{
    std::vector<GameObject*> objs = root.getObjects();
    for(int i = 0; i < objs.size(); i++)
    {
        for(int j = i + 1; j < objs.size(); j++)
        {
            GameObject::collide(objs.at(i), objs.at(j));
        }
    }

}

void Terrain::update(float dt)
{
    for(GameObject* obj : root.getObjects())
    {
        obj->update(dt);
    }

    checkCollisions();
}

std::vector<GameObject*> Terrain::getObjects()
{
    return root.getObjects();
}
