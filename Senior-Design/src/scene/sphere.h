#pragma once

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Sphere : public Drawable
{
public:
    Sphere(OpenGLContext* context) : Drawable(context){}
    virtual ~Sphere(){}
    void create() override;
};
