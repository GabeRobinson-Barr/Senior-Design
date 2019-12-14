#pragma once

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "Player.h"

class Tetherline : public Drawable
{
public:
    Tetherline(OpenGLContext* context, Player* p, PlayerGun* g) : Drawable(context), player(p), gun(g) {}
    virtual ~Tetherline(){}
    void create() override;
    GLenum drawMode() override;
    void update();

private:
    Player* player;
    PlayerGun* gun;

};
