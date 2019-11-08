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
    void addCollision(glm::vec3 collPt, GameObject* collObj);

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
    Camera* cam;
};

#endif // PLAYER_H
