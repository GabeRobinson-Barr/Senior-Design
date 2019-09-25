#include <scene/terrain.h>

#include <scene/cube.h>

Terrain::Terrain() : root(Octree(-dimensions / 2.f, dimensions / 2.f))
{}


void Terrain::CreateTestScene()
{
    // Create the basic terrain floor
    GameObject* obj1 = new GameObject(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
    GameObject* obj2 = new GameObject(glm::vec3(10,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1), 1, MeshType::SPHERE);
    root.add(obj1);
    root.add(obj2);

}

void Terrain::updateCollisions()
{

}

std::vector<GameObject*> Terrain::getObjects()
{
    return root.getObjects();
}
