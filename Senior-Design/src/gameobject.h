#pragma once
#include "drawable.h"
#include "scene/cube.h"
//#include "collider.h"
#include <vector>
#include "scene/transform.h"
#include "connectedobject.h"

static int nextId = 0;
static const float maxVel = 250.f;

enum MeshType {
    CUBE,
    SPHERE,
    MESH
};

class ConnectedObject;

class GameObject
{
public:
    GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, float mass, MeshType type);
    virtual ~GameObject();

    const int id = nextId++;

    static std::pair<bool,glm::vec3> collide(GameObject* obj1, GameObject* obj2);

    virtual void addCollision(GameObject* obj, glm::vec3 collisionPt);
    static void addStickyCollision(GameObject* obj1, GameObject* obj2, glm::vec3 collisionPt);

    // This function returns the normalized support point for a cube or sphere
    // for use in determining collision points
    glm::vec3 getSupport(glm::vec3 v);

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

    virtual void update(float dt);
    glm::vec3 getNor(glm::vec3 vec);
    virtual void addForce(glm::vec3 force, glm::vec3 collPt);
    glm::vec3 getForce();

    glm::vec3 getPos();
    glm::vec3 getRot();
    glm::vec3 getScale();
    glm::vec3 getVel();
    glm::vec3 getRotVel();
    float getMass();
    Transform getTransform();
    glm::vec4 getColor();
    void translate(glm::vec3 t);
    void setVel(glm::vec3 v);
    void setRotVel(glm::vec3 rVel);
    void updateTransform();
    void updateTransform(glm::vec3 p, glm::vec3 r, glm::vec3 s);
    void setDynamic(bool b);
    void disableOne();
    bool isDynamic;
    const MeshType geomType;
    bool addstuff = true; // used for debugging collision forces
    ConnectedObject* connectedComp; // connected component this object is part of
    bool updated = false;
    bool isSticky;
    bool noColl = false;

protected:
    glm::vec3 scale;
    glm::vec3 pos;
    glm::vec3 rot;
    Transform m_transform;
    glm::vec3 vel;
    glm::vec3 rotVel;
    float mass;
    glm::vec3 moment;
    glm::mat3 tensor;
    float drag; // drag coefficient responsible for slowing movement
    float angDrag; // angular drag coefficient responsible for slowing rotation
    glm::vec3 forces; // Total forces acting on this object
    glm::vec3 torque;
    float bounceFac = 0.5f;
    float maxSpd;


    //glm::mat4 obj_to_world;
    //glm::mat4 world_to_obj; // Transform matrix from world space

    bool hasCollision = false;
    bool hasPlayerCollision = false;

};

class ConnectedObject : public GameObject
{
public:
    ConnectedObject();
    ~ConnectedObject() override;
    void update(float dt) override;
    void addForce(glm::vec3 force, glm::vec3 collPt) override;
    static bool canCollide(GameObject* obj1, GameObject* obj2);
    void addObj(GameObject* obj);
    void removeObj(GameObject* obj);
    // Merges and returns the resulting connected object
    static ConnectedObject* mergeConnectedObjs(ConnectedObject* c1, ConnectedObject* c2);
    void removeAll();

protected:
    std::vector<GameObject*> objs;
    void updateMoment();

};
