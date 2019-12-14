#include "tetherline.h"

void Tetherline::create()
{

    GLuint idx[2] = {0, 1};
    glm::vec4 ppos = player->getCrosshair() * glm::vec4(0,0,0,1);
    glm::vec3 gpos = gun->getLastTransform().position();
    glm::vec4 pos[2] = {ppos, glm::vec4(gpos,1)};
    glm::vec4 col[2] = {glm::vec4(0,1,0,0), glm::vec4(0,1,0,0)};

    count = 2;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(GLuint), idx, GL_STATIC_DRAW);
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), pos, GL_STATIC_DRAW);
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), col, GL_STATIC_DRAW);
}

GLenum Tetherline::drawMode()
{
    return GL_LINES;
}

void Tetherline::update()
{
    context->glDeleteBuffers(1, &bufPos);
    context->glDeleteBuffers(1, &bufCol);
    glm::vec4 ppos = player->getCrosshair() * glm::vec4(0,0,0,1);
    glm::vec3 gpos = gun->getLastTransform().position();
    glm::vec4 pos[2] = {ppos, glm::vec4(gpos,1)};
    glm::vec4 col[2] = {glm::vec4(0,0,0,0), glm::vec4(0,0,0,0)};
    if(gun->isFired)
    {
        if(gun->retracting)
        {
            col[0] = glm::vec4(0,0,1,1);
            col[1] = glm::vec4(0,0,1,1);
        }
        else if (gun->connectedComp != nullptr)
        {
            col[0] = glm::vec4(0,1,0,1);
            col[1] = glm::vec4(0,1,0,1);
        }
        else
        {
            col[0] = glm::vec4(1,0,0,1);
            col[1] = glm::vec4(1,0,0,1);
        }
    }


    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), pos, GL_STATIC_DRAW);
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), col, GL_STATIC_DRAW);
}
