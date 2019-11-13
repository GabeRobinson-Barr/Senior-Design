#ifndef PLAYER_H
#define PLAYER_H
#include "gameobject.h"
#include "camera.h"

class Player : public GameObject
{
public:
    Player(Camera** c);
    ~Player() override;
    void update(float dt) override; // override to add player input forces
    void keyPressed(QKeyEvent* e);
    void keyReleased(QKeyEvent* e);
    void addForce(glm::vec3 force, glm::vec3 collPt) override; // override to update onFloor
    void addForce(glm::vec3 force);
    void addCollision(GameObject* obj, glm::vec3 collisionPt) override;
    void recomputeAttributes();
    bool objIsFloor(GameObject* obj);

protected:
    //Weapon m_weapon;
    const glm::vec3 camOffset;
    GameObject* floorObj = nullptr;
    bool onFloor = false;
    bool jumped = false;
    bool moveRight = false;
    bool blockRight = false;
    bool moveLeft = false;
    bool blockLeft = false;
    bool moveFor = false;
    bool blockFor = false;
    bool moveBack = false;
    bool blockBack = false;
    bool rotRight = false;
    bool rotLeft = false;
    bool rotUp = false;
    bool blockUp = false;
    bool rotDown = false;
    bool blockDown = false;
    float currBoost;
    float maxBoost;
    float jumpStr;
    float moveSpd;
    float rotSpd;
    float playerGrav = 0.3f;
    float speedInc = 1.f;
    Camera* cam;
    glm::vec3 playerFor;
    glm::vec3 playerRight;
    glm::vec3 playerUp;
    glm::vec3 playerSpd = glm::vec3(0.f);
    glm::vec3 forceSpd = glm::vec3(0.f);
};

#endif // PLAYER_H
