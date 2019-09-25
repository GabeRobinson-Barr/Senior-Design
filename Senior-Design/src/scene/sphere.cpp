#include "sphere.h"
#include <la.h>
#include <iostream>

static const int SPH_IDX_COUNT = 336;
static const int SPH_VERT_COUNT = 58;

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec4 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    glm::vec4 v;

    for(int i = 1; i < 8; i++)
    {
        for(int j = 0; j < 8 ; j++)
        {
            v = glm::rotate(glm::mat4(1.0f), glm::radians(j*45.0f), glm::vec3(0,1,0)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(i * -22.5f), glm::vec3(0,0,1)) *
                    glm::vec4(0,0.5f,0,1);
            sph_vert_pos[(i - 1)*8 + j + 1] = v;
        }
    }
    sph_vert_pos[0] = glm::vec4(0,0.5f, 0, 1);
    sph_vert_pos[57] = glm::vec4(0,-0.5f, 0,1);
}


void createSphereVertexNormals(glm::vec4 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    glm::vec4 v;

    for(int i = 1; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            v = glm::rotate(glm::mat4(1.0f), glm::radians(j*45.0f), glm::vec3(0,1,0)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(i * -22.5f), glm::vec3(0,0,1)) *
                    glm::vec4(0,1.0f,0,0);
            sph_vert_nor[(i - 1)*8 + j + 1] = v;
        }
    }
    sph_vert_nor[0] = glm::vec4(0,1.0f,0,0);
    sph_vert_nor[57] = glm::vec4(0,-1.0f,0,0);

}

void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    for(int i = 0; i < 7; i++)
    {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i+1;
        sph_idx[index + 2] = i+2;
        index += 3;
    }
    sph_idx[21] = 0;
    sph_idx[22] = 8;
    sph_idx[23] = 1;
    index += 3;

    for(int i = 1; i < 7; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sph_idx[index] = (i-1)*8 + j + 1;
            sph_idx[index + 1] = (i-1)*8 + j + 2;
            sph_idx[index + 2] = (i-1)*8 + j + 10;
            sph_idx[index + 3] = (i-1)*8 + j + 1;
            sph_idx[index + 4] = (i-1)*8 + j + 10;
            sph_idx[index + 5] = (i-1)*8 + j + 9;
            index += 6;
        }
    }

    for(int i = 0; i < 7; i++)
    {
        sph_idx[index] = 57;
        sph_idx[index + 1] = i+49;
        sph_idx[index + 2] = i+50;
        index += 3;
    }

    sph_idx[333] = 57;
    sph_idx[334] = 56;
    sph_idx[335] = 49;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec4 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec4 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec4 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);


    for(int i = 0; i < SPH_VERT_COUNT; i++){
        sph_vert_col[i] = glm::vec4(1,0,0, 1);
    }

    count = SPH_IDX_COUNT;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPH_IDX_COUNT * sizeof(GLuint), sph_idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, SPH_VERT_COUNT * sizeof(glm::vec4), sph_vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, SPH_VERT_COUNT * sizeof(glm::vec4), sph_vert_nor, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, SPH_VERT_COUNT * sizeof(glm::vec4), sph_vert_col, GL_STATIC_DRAW);
}
