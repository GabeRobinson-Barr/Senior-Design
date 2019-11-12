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
    void addCollision(GameObject* obj, glm::vec3 collisionPt) override;

protected:
    //Weapon m_weapon;
    const glm::vec3 camOffset;
    GameObject* floorObj = nullptr;
    bool onFloor = false;
    bool jumped = false;
    bool moveRight = false;
    bool moveLeft = false;
    bool moveFor = false;
    bool moveBack = false;
    bool rotRight = false;
    bool rotLeft = false;
    float currBoost;
    float maxBoost;
    float jumpStr;
    float moveSpd;
    float rotSpd;
    float playerGrav = 0.3f;
    Camera* cam;
    glm::vec3 playerUp;
};

#endif // PLAYER_H
