#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "drawable.h"
#include "scene/cube.h"
#include "collider.h"


class GameObject
{
public:
    GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, Drawable* geom);
    ~GameObject();

    void update(float dt);
    void addForce(glm::vec3 force);
    glm::vec3 getForce();


private:
    Drawable* mp_geometry;
    glm::vec3 scale;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 vel;
    glm::vec3 rotVel;
    float mass;
    glm::vec3 forces; // Total forces acting on this object

    glm::mat4 obj_to_world;
    glm::mat4 world_to_obj; // Transform matrix from world space

    bool hasCollision;
    bool updated;

    std::vector<Collider*> colliders; // List of collisions involving this object
    // RETHINK THIS/ASK PROF JANG
    glm::vec3 centerOfMass; // CoM of all sticky objects collided with this
    float connectedMass; // total mass of sticky objects

};

#endif // GAMEOBJECT_H
