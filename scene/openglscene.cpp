#include "openglscene.h"
#include "camera.h"
#include "ResourceLoader.h"
#include "view.h"
#include <string>
#include <sstream>
#include "QCoreApplication"
#include <QFile>
#include "test.h"
#include "room.h"

#define SHAPE_RADIUS 0.5f

OpenGLScene::OpenGLScene()
{
    m_drawWireframe = false;
    m_useLighting = true;
}

OpenGLScene::~OpenGLScene()
{
    // delete primitives
    int i;
    int num = m_elements.size();
    for (i = 0; i < num; i++)
    {
        SceneElement *e = m_elements.at(i);
        delete e->primitive->material.bumpMap;
        delete e->primitive->material.textureMap;
        delete e->primitive;
        delete e;
    }
    m_elements.clear();

    // delete lights
    num = m_lights.size();
    for (i = 0; i < num; i++)
    {
        CS123SceneLightData *l = m_lights.at(i);
        delete l;
    }
    m_lights.clear();

    delete m_room;
}


void OpenGLScene::init()
{
    m_solidShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/default.vert",
                ":/shaders/default.gsh",
                ":/shaders/default.frag");
    m_testShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/default.vert",
                ":/shaders/cube.gsh",
                ":/shaders/default.frag");
    m_cubeShader = ResourceLoader::loadShaders(
                ":/shaders/cube.vert",
                ":/shaders/cube.frag");
    m_waterShader = ResourceLoader::loadShaders(
                ":/shaders/glass.vert",
                ":/shaders/glass.frag");

    // solids
    m_solidUniforms["projection"]= glGetUniformLocation(m_solidShader, "projection");
    m_solidUniforms["view"]= glGetUniformLocation(m_solidShader, "view");
    m_solidUniforms["model"]= glGetUniformLocation(m_solidShader, "model");
    m_solidUniforms["ambient_color"] = glGetUniformLocation(m_solidShader, "ambient_color");
    m_solidUniforms["diffuse_color"] = glGetUniformLocation(m_solidShader, "diffuse_color");
    m_solidUniforms["specular_color"] = glGetUniformLocation(m_solidShader, "specular_color");
    m_solidUniforms["shininess"] = glGetUniformLocation(m_solidShader, "shininess");
    m_solidUniforms["repeatU"] = glGetUniformLocation(m_solidShader, "repeatU");
    m_solidUniforms["repeatV"] = glGetUniformLocation(m_solidShader, "repeatV");
    m_solidUniforms["useLighting"]= glGetUniformLocation(m_solidShader, "useLighting");
    m_solidUniforms["useArrowOffsets"] = glGetUniformLocation(m_solidShader, "useArrowOffsets");
    m_solidUniforms["allBlack"]= glGetUniformLocation(m_solidShader, "allBlack");
    m_solidUniforms["allWhite"]= glGetUniformLocation(m_solidShader, "allWhite");
    m_solidUniforms["functionSize"]= glGetUniformLocation(m_solidShader, "functionSize");
    m_solidUniforms["function"]= glGetUniformLocation(m_solidShader, "function");
    m_solidUniforms["tex"] = glGetUniformLocation(m_solidShader, "tex");
    m_solidUniforms["useTexture"] = glGetUniformLocation(m_solidShader, "useTexture");

    // cube
    m_cubeUniforms["projection"] = glGetUniformLocation(m_cubeShader, "projection");
    m_cubeUniforms["view"] = glGetUniformLocation(m_cubeShader, "view");
    m_cubeUniforms["envMap"] = glGetUniformLocation(m_cubeShader, "envMap");

    // water
    m_waterUniforms["view"] = glGetUniformLocation(m_waterShader, "view");
    m_waterUniforms["projection"] = glGetUniformLocation(m_waterShader, "projection");
    m_waterUniforms["model"] = glGetUniformLocation(m_waterShader, "model");
    m_waterUniforms["functionSize"] = glGetUniformLocation(m_waterShader, "functionSize");
    m_waterUniforms["function"] = glGetUniformLocation(m_waterShader, "function");
    m_waterUniforms["r0"] = glGetUniformLocation(m_waterShader, "r0");
    m_waterUniforms["eta"] = glGetUniformLocation(m_waterShader, "eta");

//    m_test = new Test();
//    m_test->init();

    m_room = new Room(25.f);
    m_room->init();
    m_room->makeCubeMap();
}

void OpenGLScene::render(Camera *cam, bool test)
{
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
    if (test) m_room->bindFramebuffer();
    else glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the camera
    assert(cam);
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();

//    // cubemap
//    glUseProgram(m_cubeShader);
//    glDepthMask(GL_FALSE);

//    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "projection"), 1, GL_FALSE,
//            glm::value_ptr(projMatrix));
//    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "view"), 1, GL_FALSE,
//                       glm::value_ptr(viewMatrix));
////    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "view"), 1, GL_FALSE,
////                       glm::value_ptr(glm::rotate(viewMatrix, (float) M_PI, glm::vec3(0, 1, 0))));
//    glUniform1i(glGetUniformLocation(m_cubeShader, "envMap"), 1);

//    renderSetting();
//    m_room->render();

//    glDepthMask(GL_TRUE);

    // solids
    GLuint shader;
    if (test) {
        shader = m_testShader;
        glUseProgram(shader);
        m_room->bindTexture();
        m_room->setProjections(shader);
    }
    else {
        shader = m_solidShader;
        glUseProgram(shader);
    }

//    if (test) {
    // Set scene uniforms.
    clearLights(shader);
    setLights(viewMatrix, shader);
    glUniform1i(glGetUniformLocation(shader, "useLighting"), m_useLighting);
    glUniform1i(glGetUniformLocation(shader, "useArrowOffsets"), GL_FALSE);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniform3f(glGetUniformLocation(shader, "allBlack"), 1, 1, 1);

    renderSolids(shader);

    // water
    if (!test) {
        shader = m_waterShader;
        glUseProgram(shader);

        // water stuffs
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));
        glUniform1i(glGetUniformLocation(shader, "envMap"), 1);

        glActiveTexture(GL_TEXTURE1);
        m_room->bindTexture();
        glActiveTexture(GL_TEXTURE0);

        renderTransparents(shader);
    }
//    }

    // cubemap
    if (!test) {
        glUseProgram(m_cubeShader);
        glDepthMask(GL_FALSE);

        glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "projection"), 1, GL_FALSE,
                glm::value_ptr(projMatrix));
        glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "view"), 1, GL_FALSE,
                           glm::value_ptr(viewMatrix));
        glUniform1i(glGetUniformLocation(m_cubeShader, "envMap"), 1);

        m_room->render();

        glDepthMask(GL_TRUE);
    }
    glUseProgram(0);
}


void OpenGLScene::applyMaterial(const CS123SceneMaterial &material, GLuint shader)
{
    // Make sure the members of CS123SceneColor are packed tightly
    COMPILE_TIME_ASSERT(sizeof(CS123SceneColor) == sizeof(float) * 4);

    glUniform3fv(glGetUniformLocation(shader, "ambient_color"), 1, &material.cAmbient.r);
    glUniform3fv(glGetUniformLocation(shader, "diffuse_color"), 1, &material.cDiffuse.r);
    glUniform3fv(glGetUniformLocation(shader, "specular_color"), 1, &material.cSpecular.r);
    glUniform1f(glGetUniformLocation(shader, "shininess"), material.shininess);

    if (material.textureMap && material.textureMap->isUsed && material.textureMap->texid) {
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shader, "tex"), 1);
        glUniform1f(glGetUniformLocation(shader, "repeatU"), material.textureMap->repeatU);
        glUniform1f(glGetUniformLocation(shader, "repeatV"), material.textureMap->repeatV);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.textureMap->texid);
        glActiveTexture(GL_TEXTURE0);
    } else {
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    }
}

void OpenGLScene::clearLights(GLuint shader)
{
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        glUniform3f(glGetUniformLocation(shader, ("lightColors" + indexString).c_str()), 0, 0, 0);
    }
}

void OpenGLScene::setLight(const CS123SceneLightData &light, GLuint shader)
{
    std::ostringstream os;
    os << light.id;
    std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.

    bool ignoreLight = false;

    GLint lightType;
    switch(light.type)
    {
    case LIGHT_POINT:
        lightType = 0;
        glUniform3fv(glGetUniformLocation(shader, ("lightPositions" + indexString).c_str()), 1,
                glm::value_ptr(light.pos));
        break;
    case LIGHT_DIRECTIONAL:
        lightType = 1;
        glUniform3fv(glGetUniformLocation(shader, ("lightDirections" + indexString).c_str()), 1,
                glm::value_ptr(glm::normalize(light.dir)));
        break;
    default:
        lightType = -1;
        ignoreLight = true; // Light type not supported
        break;
    }

    CS123SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    glUniform1i(glGetUniformLocation(shader, ("lightTypes" + indexString).c_str()), lightType);
    glUniform3f(glGetUniformLocation(shader, ("lightColors" + indexString).c_str()),
                color.r, color.g, color.b);
    glUniform3f(glGetUniformLocation(shader, ("lightAttenuations" + indexString).c_str()),
            light.function.x, light.function.y, light.function.z);
}


// Copied from lab04
int OpenGLScene::loadTexture(const QString &filename)
{
    // make sure file exists
    QFile file(filename);
    if (!file.exists())
        return -1;

    // load file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);
    QImage texture = QGLWidget::convertToGLFormat(image);

    // generate texture ID
    GLuint id = 0;
    glGenTextures(1, &id);

    // make the texture
    glBindTexture(GL_TEXTURE_2D, id);

    // copy image data into texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    // filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // coordinate wrapping options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return id;
}
