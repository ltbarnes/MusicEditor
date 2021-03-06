#include "cubemap.h"

CubeMap::CubeMap()
{
}


CubeMap::~CubeMap()
{
}


void CubeMap::makeCubeMap(QHash<GLenum, QImage> images)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_texID);

    setImages(images);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void CubeMap::bindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}


void CubeMap::bindTexture()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
}


void CubeMap::setImages(QHash<GLenum, QImage> images)
{
    GLenum side;
    QImage texture;
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);

    for (int i = 0; i < 6; i++) {
        side = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        texture = images.value(side);

        glTexImage2D(side, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    }
}


void CubeMap::setModel(GLuint shader, glm::mat4 trans)
{
    glm::mat4 inv = glm::mat4();
    inv[3] = glm::vec4(-glm::vec3(trans[3]), 1.f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "waterModel"), 1, GL_FALSE, glm::value_ptr(inv));
}
