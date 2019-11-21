#ifndef PLAYERGUN_H
#define PLAYERGUN_H
#include "gameobject.h"
#include "player.h"

class GunRope;

class PlayerGun : public GameObject
{
public:
    PlayerGun(Player* p);
    ~PlayerGun() override;
    void update(float dt) override;
    glm::vec3 getNor(glm::vec3 vec);
    void addForce(glm::vec3 force, glm::vec3 collPt) override;
    void addCollision(GameObject* obj, glm::vec3 collisionPt) override;
    void fire(glm::vec3 castVec);
    void retract();

protected:
    Player* myPlayer;
    glm::vec3 castVec;
    float fireSpd;

};

class GunRope
{
public:
    GunRope(PlayerGun* pg, GunRope* last, GunRope* next, glm::vec3 castPos, glm::vec3 castVec);
    void moveNext(glm::vec3 force);

private:
    PlayerGun* myGun;
    GunRope* nextRope;
    GunRope* lastRope;
    glm::vec3 originalPos;
    glm::vec3 castVec;
    glm::vec3 vel;
    float xDiff;
    float yDiff;

};

#endif // PLAYERGUN_H
