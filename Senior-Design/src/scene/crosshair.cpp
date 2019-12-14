#include "crosshair.h"

void Crosshair::create()
{

    GLuint idx[4] = {0, 1, 2, 3};
    glm::vec4 pos[4] = {glm::vec4(-0.02f,0,0,1), glm::vec4(0.02f,0,0,1),
                        glm::vec4(0,-0.025f,0,1), glm::vec4(0,0.025f,0,1)};
    glm::vec4 col[4] = {glm::vec4(0.75f,0,0,1), glm::vec4(0.75f,0,0,1),
                        glm::vec4(0.75f,0,0,1), glm::vec4(0.75f,0,0,1)};

    count = 6;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), idx, GL_STATIC_DRAW);
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), pos, GL_STATIC_DRAW);
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), col, GL_STATIC_DRAW);
}

GLenum Crosshair::drawMode()
{
    return GL_LINES;
}
