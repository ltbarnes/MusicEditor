#ifndef GRID_H
#define GRID_H

#include "openglscene.h"

class Grid
{
public:
    Grid(float radius);
    virtual ~Grid();

    void updateGL(GLuint shader);
    void cleanUp();

    virtual void calcVerts();
    void render();
    void transformAndRender(GLuint shader, glm::mat4 trans);

    virtual bool animate();

    float map(const float val, const float oldMin, const float oldMax, const float newMin, const float newMax);
    glm::vec2 mapPoints(const glm::vec2 val, const glm::vec2 oldMin, const glm::vec2 oldMax,
                        const glm::vec2 newMin, const glm::vec2 newMax);

protected:

    void addVertex(int *i, glm::vec3 v, glm::vec3 norm);
    void addVertexT(int *i, glm::vec3 v, glm::vec3 norm, glm::vec2 tex);

    void printVert(glm::vec3 v);
    void setParamMax(int p1, int p2, float p3);

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;
    GLfloat *m_vertexData;

    int m_numVerts;

private:
    float m_radius;
};

#endif // GRID_H
