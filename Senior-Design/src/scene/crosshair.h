#pragma once

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Crosshair : public Drawable
{
public:
    Crosshair(OpenGLContext* context) : Drawable(context){}
    virtual ~Crosshair(){}
    void create() override;
    GLenum drawMode() override;
};
