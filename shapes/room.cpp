#include "room.h"
#include <QGLWidget>
#include <QFile>

#define FRONT ":/images/front.png"
#define BACK ":/images/back.png"
#define BOTTOM ":/images/bottom.png"
#define TOP ":/images/top.png"
#define LEFT ":/images/left.png"
#define RIGHT ":/images/right.png"


Room::Room(float radius)
{
    m_radius = radius;
}


Room::~Room()
{
}


void Room::init()
{
    float points[] = {
        -m_radius ,  m_radius,  m_radius, // 4
         m_radius ,  m_radius,  m_radius, // 3
        -m_radius , -m_radius,  m_radius, // 7
         m_radius , -m_radius,  m_radius, // 8

         m_radius , -m_radius, -m_radius, // 5
         m_radius ,  m_radius,  m_radius, // 3
         m_radius ,  m_radius, -m_radius, // 1

        -m_radius ,  m_radius,  m_radius, // 4
        -m_radius ,  m_radius, -m_radius, // 2
        -m_radius , -m_radius,  m_radius, // 7

        -m_radius , -m_radius, -m_radius, // 6
         m_radius , -m_radius, -m_radius, // 5
        -m_radius ,  m_radius, -m_radius, // 2
         m_radius ,  m_radius, -m_radius  // 1
    };
    glGenBuffers (1, &m_vboID);
    glBindBuffer (GL_ARRAY_BUFFER, m_vboID);
    glBufferData (GL_ARRAY_BUFFER, 3 * 14 * sizeof (float), &points, GL_STATIC_DRAW);

    glGenVertexArrays (1, &m_vaoID);
    glBindVertexArray (m_vaoID);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, m_vboID);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}


void Room::makeCubeMap()
{
    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_texID);

    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_X, QString::fromStdString(RIGHT)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, QString::fromStdString(LEFT)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, QString::fromStdString(TOP)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, QString::fromStdString(BOTTOM)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, QString::fromStdString(BACK)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, QString::fromStdString(FRONT)));

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void Room::render()
{
    bindTexture();

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    glBindVertexArray(0);
}


void Room::bindTexture()
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
    glActiveTexture(GL_TEXTURE0);
}


bool Room::loadTexture(GLuint tex, GLenum side, const QString &filename)
{
    // make sure file exists
    QFile file(filename);
    if (!file.exists())
        return false;

    // load file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);
    QImage texture = QGLWidget::convertToGLFormat(image);

    // make the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    glTexImage2D(side, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    return true;
}
