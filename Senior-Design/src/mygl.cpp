#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      mp_geomCube(new Cube(this)), mp_geomSphere(new Sphere(this)), mp_worldAxes(new WorldAxes(this)), mp_crosshair(new Crosshair(this)),
      mp_progLambert(new ShaderProgram(this)), mp_progFlat(new ShaderProgram(this)),
      mp_camera(new Camera()), mp_terrain(new Terrain()), paused(false), player1(new Player(&mp_camera))
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
    grabKeyboard();
    mp_terrain->addPlayer(player1);
    p1Gun = dynamic_cast<PlayerGun*>(player1->getGun());
    mp_tether = new Tetherline(this, player1, p1Gun);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    mp_geomCube->destroy();
    mp_geomSphere->destroy();
    mp_worldAxes->destroy();
    mp_crosshair->destroy();
    mp_tether->destroy();

    delete mp_geomCube;
    delete mp_geomSphere;
    delete mp_worldAxes;
    delete mp_progLambert;
    delete mp_progFlat;
    delete mp_camera;
    delete mp_terrain;
    delete mp_crosshair;
    delete mp_tether;
}


void MyGL::MoveMouseToCenter()
{
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.1f, 0.1f, 0.15f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    mp_geomCube->create();
    mp_geomSphere->create();
    mp_worldAxes->create();
    mp_crosshair->create();
    mp_tether->create();

    // Create and set up the diffuse shader
    mp_progLambert->create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    mp_progFlat->create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    mp_progLambert->setGeometryColor(glm::vec4(1,0,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

    mp_terrain->CreateTestScene();
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    //*mp_camera = Camera(w, h, glm::vec3(mp_terrain->dimensions.x, mp_terrain->dimensions.y * 0.75, mp_terrain->dimensions.z),
                       //glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y / 2, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));

    //*mp_camera = Camera(w, h, glm::vec3(50, 50, 100),
    //                   glm::vec3(10,0,0), glm::vec3(0,1,0));
    glm::mat4 viewproj = mp_camera->getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    mp_progLambert->setViewProjMatrix(viewproj);
    mp_progFlat->setViewProjMatrix(viewproj);

    printGLErrorLog();
}


// MyGL's constructor links timerUpdate() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to use timerUpdate
void MyGL::timerUpdate()
{
    if(!paused)
    {
        mp_terrain->update((float)timer.interval() / 1000.f);
        player1->update((float)timer.interval() / 1000.f);
    }

    /*float amount = 2.0f;
    if (key_Right) {
        mp_camera->RotateAboutUp(-amount);
    }
    if (key_Left) {
        mp_camera->RotateAboutUp(amount);
    }
    if (key_Up) {
        mp_camera->RotateAboutRight(-amount);
    }
    if (key_Down) {
        mp_camera->RotateAboutRight(amount);
    }
    if (key_1) {
        mp_camera->fovy += amount;
    }
    if (key_2) {
        mp_camera->fovy -= amount;
    }
    if (key_W) {
        mp_camera->TranslateAlongLook(amount);
    }
    if (key_S) {
        mp_camera->TranslateAlongLook(-amount);
    }
    if (key_D) {
        mp_camera->TranslateAlongRight(amount);
    }
    if (key_A) {
        mp_camera->TranslateAlongRight(-amount);
    }
    if (key_Q) {
        mp_camera->TranslateAlongUp(-amount);
    }
    if (key_E) {
        mp_camera->TranslateAlongUp(amount);
    }
    mp_camera->RecomputeAttributes();*/

    update();

}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mp_progFlat->setViewProjMatrix(mp_camera->getViewProj());
    mp_progLambert->setViewProjMatrix(mp_camera->getViewProj());

    GLDrawScene();

    glDisable(GL_DEPTH_TEST);
    mp_progFlat->setModelMatrix(glm::mat4());
    mp_progFlat->draw(*mp_worldAxes);
    if(p1Gun->isFired)
    {
        mp_tether->update();
        mp_progFlat->draw(*mp_tether);
    }
    mp_progFlat->setModelMatrix(player1->getCrosshair());
    mp_progFlat->draw(*mp_crosshair);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::GLDrawScene()
{
    for(GameObject* obj : mp_terrain->getObjects())
    {
        mp_progLambert->setModelMatrix(obj->getLastTransform().T());
        Drawable* geom;
        if (obj->geomType == MeshType::CUBE)
        {
            geom = mp_geomCube;
        }
        else if (obj->geomType == MeshType::SPHERE)
        {
            geom = mp_geomSphere;
        }
        else
        {
            // do something for mesh types
        }
        mp_progLambert->setGeometryColor(obj->getColor());

        mp_progLambert->draw(*geom);
    }
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_P) {
        paused = !paused;
    }
    else {
        player1->keyPressed(e);
    }

}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
    else {
        player1->keyReleased(e);
    }
}
