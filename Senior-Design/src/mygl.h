#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include <scene/sphere.h>
#include <scene/worldaxes.h>
#include "camera.h"
#include <scene/terrain.h>
#include "player.h"
#include "playergun.h"
#include <scene/crosshair.h>
#include <scene/tetherline.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL : public OpenGLContext
{
    Q_OBJECT
private:
    Cube* mp_geomCube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    Sphere* mp_geomSphere;
    WorldAxes* mp_worldAxes; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    Crosshair* mp_crosshair;
    Tetherline* mp_tether;
    ShaderProgram* mp_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram* mp_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera* mp_camera;
    Terrain* mp_terrain;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;

    void MoveMouseToCenter(); // Forces the mouse position to the screen's center. You should call this
                              // from within a mouse move event after reading the mouse movement so that
                              // your mouse stays within the screen bounds and is always read.
    bool paused;
    Player* player1;
    PlayerGun* p1Gun;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    bool key_Right = false;
    bool key_Left = false;
    bool key_Up = false;
    bool key_Down = false;
    bool key_1 = false;
    bool key_2 = false;
    bool key_W = false;
    bool key_S = false;
    bool key_A = false;
    bool key_D = false;
    bool key_Q = false;
    bool key_E = false;


private slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();
};


#endif // MYGL_H
