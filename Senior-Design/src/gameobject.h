#pragma once
#include "drawable.h"
#include "scene/cube.h"
//#include "collider.h"
#include <vector>
#include "scene/transform.h"

enum MeshType {
    CUBE,
    SPHERE,
    MESH
};

class Collider;

class GameObject
{
public:
    GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type);
    ~GameObject();

    static void collide(GameObject* obj1, GameObject* obj2);

    // These 2 functions return the normalized support point for a cube or sphere
    // for use in determining collision points
    glm::vec3 getCubeSupport(glm::vec3 v);
    glm::vec3 getSphereSupport(glm::vec3 v);

    // Helper function that returns the sign of an input number
    template<typename T>
    static int sgn(T a)
    {
        if (a > 0)
        {
            return 1;
        }
        else if (a < 0)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    void update(float dt);
    void addForce(glm::vec3 force);
    glm::vec3 getForce();

    glm::vec3 getPos();
    glm::mat4 getTransform();
    glm::vec4 getColor();
    const MeshType geomType;

private:
    glm::vec3 scale;
    glm::vec3 pos;
    glm::vec3 rot;
    Transform m_transform;
    glm::vec3 vel;
    glm::vec3 rotVel;
    float mass;
    float drag; // drag coefficient responsible for slowing movement
    float angDrag; // angular drag coefficient responsible for slowing rotation
    glm::vec3 forces; // Total forces acting on this object

    //glm::mat4 obj_to_world;
    //glm::mat4 world_to_obj; // Transform matrix from world space

    bool hasCollision = false;
    bool updated = false;

    std::vector<Collider*> colliders; // List of collisions involving this object
    // RETHINK THIS/ASK PROF JANG
    glm::vec3 centerOfMass; // CoM of all sticky objects collided with this
    float connectedMass; // total mass of sticky objects

};
