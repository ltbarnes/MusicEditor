#include "scene.h"
#include "room.h"
#include "grid.h"
#include "musicshape.h"
#include "filtershape.h"
#include "udphandler.h"
#include "glm/gtx/vector_angle.hpp"
#include "cubemap.h"

glm::vec4 lightDirection = glm::normalize(glm::vec4(1.f, -1.f, 1.f, 0.f));

Scene::Scene(QObject *parent)
{
    m_global.ka = 0.8f;
    m_global.kd = 0.8f;
    m_global.ks = 0.8f;
    m_global.kt = 1.f;

    // set shape pointer
    m_room = NULL;
    m_grid = NULL;
    m_lightningShape = NULL;
    m_waterShape = NULL;
    m_filter = NULL;

    m_lights.clear();
    m_waterElements.clear();
    m_lightningElements.clear();

    // music data
    m_udp1 = new UDPHandler(this, SLOT(setF1(QVector<float>)), 7001);
    m_udp2 = new UDPHandler(this, SLOT(setF2(QVector<float>)), 7002);
    m_udp3 = new UDPHandler(this, SLOT(setF3(QVector<float>)), 7003);
    m_udp5 = new UDPHandler(this, SLOT(setF5(QVector<float>)), 7005);
    m_udp6 = new UDPHandler(this, SLOT(setF6(QVector<float>)), 7006);
    m_udp7 = new UDPHandler(this, SLOT(setF7(QVector<float>)), 7007);
    m_udp8 = new UDPHandler(this, SLOT(setF8(QVector<float>)), 7008);

    m_unused.append(7005);
    m_unused.append(7006);
    m_unused.append(7007);
    m_unused.append(7008);

    setUp();

    m_initialized = false;
}

Scene::~Scene()
{
    delete m_udp1;
    delete m_udp2;
    delete m_udp3;
    delete m_udp5;
    delete m_udp6;
    delete m_udp7;
    delete m_udp8;
}


void Scene::sendMusicData(glm::vec4 eye)
{
    glm::vec3 e = glm::vec3(eye);
    e.y = 0;

    bool drumOneOff = true;
    bool drumTwoOff = true;

    SceneElement *element;
    int num = m_waterElements.size();
    glm::vec2 d;
    glm::vec3 v1;
    glm::vec3 v2 = glm::vec3(0) - e;
    v2.y = 0;
    for (int i = 0; i < num; i++) {
        element = m_waterElements.value(i);
        d.x = glm::distance(glm::vec3(element->trans[3]), e);

        v1 = glm::vec3(element->trans[3]) - e;
        v1.y = 0;

        if (d.x < .1f)
            d.y = 0.5f;
        else {
            d.y = glm::angle(glm::normalize(v1), glm::normalize(v2));
            d.y /= (M_PI / 2.f);
            int cross = (glm::cross(v1, v2).y > 0 ? 1 : -1);
            d.y = .5 + (cross * d.y);
            d.x = glm::clamp(1.f - d.x / 10.f, 0.f, 1.f);
        }

        switch (element->port) {
        case 7001:
            drumOneOff = false;
            m_udp1->sendInfo(d.x, d.y, element->linked);
            break;
        case 7002:
            drumTwoOff = false;
            m_udp2->sendInfo(d.x, d.y, element->linked);
            break;
        case 7005:
            m_udp5->sendInfo(d.x, d.y, element->linked);
            break;
        case 7006:
            m_udp6->sendInfo(d.x, d.y, element->linked);
            break;
        case 7007:
            m_udp7->sendInfo(d.x, d.y, element->linked);
            break;
        case 7008:
            m_udp8->sendInfo(d.x, d.y, element->linked);
            break;
        default:
            break;
       }
    }
    element = m_input;
    d.x = glm::distance(glm::vec3(element->trans[3]), e);

    v1 = glm::vec3(element->trans[3]) - e;
    v1.y = 0;
    d.y = glm::angle(glm::normalize(v1), glm::normalize(v2));
    d.y /= (M_PI / 2.f);
    int cross = (glm::cross(v1, v2).y > 0 ? 1 : -1);
    d.y = .5 + (cross * d.y);
    d.x = glm::clamp(1.f - d.x / 10.f, 0.f, 1.f);
    m_udp3->sendInfo(d.x, d.y, element->linked);

    if (drumOneOff)
        m_udp1->sendInfo(0.f, 0.5f, false);
    if (drumTwoOff)
        m_udp2->sendInfo(0.f, 0.5f, false);

    foreach (int port, m_unused) {
        switch (port) {
        case 7005:
            m_udp5->sendInfo(0.f, 0.5f, false);
            break;
        case 7006:
            m_udp6->sendInfo(0.f, 0.5f, false);
            break;
        case 7007:
            m_udp7->sendInfo(0.f, 0.5f, false);
            break;
        case 7008:
            m_udp8->sendInfo(0.f, 0.5f, false);
            break;
        }
    }
}


void Scene::setUp()
{
    CS123ScenePrimitive *prim = new CS123ScenePrimitive();
    CS123SceneMaterial& mat = prim->material;

    prim->meshfile = "";
  //  prim->type = PRIMITIVE_CUBE;

    // Use a shiny orange material
    memset(&mat, 0, sizeof(CS123SceneMaterial));
    mat.cAmbient.r = 0.2f;
    mat.cAmbient.g = 0.2f;
    mat.cAmbient.b = 0.2f;
    mat.cDiffuse.r = 0.5f;
    mat.cDiffuse.g = 0.5f;
    mat.cDiffuse.b = 0.5f;
    mat.cSpecular.r = mat.cSpecular.g = mat.cSpecular.b = 1;
    mat.shininess = 64;

    // Use snow texture
    mat.textureMap = new CS123SceneFileMap();
//    mat.textureMap->filename = "/course/cs123/data/image/terrain/snow.JPG";
    mat.textureMap->filename = "/Users/Logan/Documents/course/cs123/data/image/terrain/snow.JPG";
    mat.textureMap->isUsed = false;
    mat.textureMap->repeatU = 1;
    mat.textureMap->repeatV = 1;
    mat.blend = 0.5f;

    mat.bumpMap = new CS123SceneFileMap();
    mat.bumpMap->filename = "";
    mat.bumpMap->isUsed = false;
    mat.bumpMap->repeatU = 1;
    mat.bumpMap->repeatV = 1;
    mat.bumpMap->texid = 0;

    CS123SceneLightData *light = new CS123SceneLightData();
    // Use a white directional light from the upper left corner
    memset(light, 0, sizeof(CS123SceneLightData));
    light->type = LIGHT_POINT;
//    light->pos = glm::vec4(-5.f, 8.f, -10.f, 1.f);
//    light->color.r = light->color.g = light->color.b = 1.f;
    light->pos = glm::vec4(-10.f, 10.f, -10.f, 1);
    light->color.r = 1.0f;
    light->color.g = 0.7f;
    light->color.b = 0.5f;
    light->id = 0;

    SceneElement *element = new SceneElement();
    element->dragged = false;
    element->render = true;
    element->linked = false;
    element->link = -1;

    prim->type = LIGHTNING_TYPE;
    element->primitive = prim;
//    element->trans = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));
    element->trans = glm::translate(glm::vec3(0.f, 8.f, -25.f));
//    element->trans = glm::mat4();
//    element->inv = glm::mat4();
    element->inv = glm::inverse(element->trans);
    m_lights.append(light);
    m_lightningElements.append(element);

//    glm::mat4x4 rots = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));

    element = new SceneElement();
    element->dragged = false;
    element->render = true;
    element->link = -1;
    element->linked = false;
    element->port = 7001;
    prim->type = WATER_TYPE;
    element->primitive = prim;
    element->trans = glm::translate(glm::vec3(-1.0f, 1.f, -5.5f));
    element->inv = glm::inverse(element->trans);
    m_waterElements.append(element);

    element = new SceneElement();
    element->dragged = false;
    element->render = true;
    element->link = -1;
    element->linked = false;
    element->port = 7002;
    prim->type = WATER_TYPE;
    element->primitive = prim;
    element->trans = glm::translate(glm::vec3(1.f, 1.f, -5.5f));
    element->inv = glm::inverse(element->trans);
    m_waterElements.append(element);

    m_input = new SceneElement();
    m_input->dragged = false;
    m_input->render = true;
    m_input->link = -1;
    m_input->linked = false;
    m_input->port = 7003;
    prim->type = WATER_TYPE;
    m_input->primitive = prim;
    m_input->trans = glm::translate(glm::vec3(0, 0, 0));
    m_input->inv = glm::inverse(m_input->trans);

       // glUniform3f(glGetUniformLocation(m_shader, "allWhite"), 0, 0, 0); // not white
     //   m_shape->transformAndRender(m_shader, glm::translate(glm::mat4(), glm::vec3(2, 0, 0)));

    m_initialized = false;

}


void Scene::init()
{
    if (m_initialized)
        return;

    OpenGLScene::init(); // Call the superclass's init()

//    m_room = new Room(5.f);
//    m_room->init();
//    m_room->makeCubeMap();

    m_grid = new Grid(5.f);
    m_grid->calcVerts();
    m_grid->updateGL(m_solidShader);
    m_grid->cleanUp();

    m_lightningShape = new MusicShape(150, 70, 0.15f);
    m_lightningShape->calcVerts();
    m_lightningShape->updateGL(m_orbShader);
    m_lightningShape->cleanUp();
    m_lightningShape->m_isWater = false;

//    m_waterShape = new MusicShape(150, 70, 0.15f);
    m_waterShape = new MusicShape(4, 4, 0.15f);
    m_waterShape->calcVerts();
    m_waterShape->updateGL(m_waterShader);
    m_waterShape->cleanUp();
    m_waterShape->m_isWater = true;

    m_filter = new FilterShape(50, 50, 20, 0.3f);
    m_filter->calcVerts();
    m_filter->updateGL(m_solidShader);
    m_filter->cleanUp();

//    CS123SceneMaterial& mat = m_waterElements.at(0)->primitive->material;
//    int texId = loadTexture(QString::fromStdString(mat.textureMap->filename));
//    if (texId == -1) {
//        cout << "Texture '" << mat.textureMap->filename << "' does not exist" << endl;
//        mat.textureMap->isUsed = 0;
//    } else {
//        mat.textureMap->texid = texId;
//        mat.textureMap->isUsed = true;
//    }

    m_initialized = true;

//    foreach(SceneElement *e, m_waterElements) {
//        e->cube = new CubeMap();
//        e->cube->makeCubeMap(m_images);
//    }
}


void Scene::renderSetting()
{

    if (!m_initialized)
        return;

//    m_room->render();
}

void Scene::deleteObject(PrimitiveType typ, int ind) {
    if (typ == WATER_TYPE) {
//        if () {
//            return;
//        }
//        if (m_waterElements.at(ind)->linked) {
//            int lInd = m_waterElements.at(ind)->link;

//            m_waterElements.at(ind)->linked = false;
//            m_lightningElements.at(lInd)->render = true;
//            m_waterElements.at(ind)->link = -1;
//            m_lightningElements.at(lInd)->trans = m_waterElements.at(ind)->trans;
//        }
        m_deleteElements.append(m_waterElements.value(ind));
        int port = m_waterElements.value(ind)->port;
        if (port != 7001 && port != 7002 && port != 7003 && port != 7004)
            m_unused.append(m_waterElements.value(ind)->port);
        m_waterElements.removeAt(ind);
//        delete e->primitive->material.bumpMap;
//        delete e->primitive->material.textureMap;
//        delete e->primitive;
//        delete e;
    }
    else if (typ == LIGHTNING_TYPE) {
        if (m_lightningElements.size() == 1) {
            return;
        }
        int lInd = ind;
        for (int i = 0; i < m_waterElements.size(); ++i) {
            if (m_waterElements.at(i)->linked) {
                int lInd2 = m_waterElements.at(i)->link;
                if (lInd2 > lInd) {
                    m_waterElements.at(i)->link = (lInd2 - 1);
                }
            }
        }
        m_deleteElements.append(m_lightningElements.at(ind));
        m_lightningElements.removeAt(ind);
//        delete e->primitive->material.bumpMap;
//        delete e->primitive->material.textureMap;
//        delete e->primitive;
//        delete e;
    }
}

void Scene::addObject(PrimitiveType typ) {
    if (typ == WATER_TYPE) {
        if (m_unused.isEmpty()) {
            return;
        }
        else {
            CS123ScenePrimitive *prim = new CS123ScenePrimitive();
            CS123SceneMaterial& mat = prim->material;

            prim->meshfile = "";
          //  prim->type = PRIMITIVE_CUBE;

            // Use a shiny orange material
            memset(&mat, 0, sizeof(CS123SceneMaterial));
            mat.cAmbient.r = 0.2f;
            mat.cAmbient.g = 0.2f;
            mat.cAmbient.b = 0.4f;
            mat.cDiffuse.r = 0.5f;
            mat.cDiffuse.g = 0.5f;
            mat.cDiffuse.b = 1.0f;
            mat.cSpecular.r = mat.cSpecular.g = mat.cSpecular.b = 1;
            mat.shininess = 64;

            // Use snow texture
            mat.textureMap = new CS123SceneFileMap();
        //    mat.textureMap->filename = "/course/cs123/data/image/terrain/snow.JPG";
            mat.textureMap->filename = "/Users/Logan/Documents/course/cs123/data/image/terrain/snow.JPG";
            mat.textureMap->isUsed = false;
            mat.textureMap->repeatU = 1;
            mat.textureMap->repeatV = 1;
            mat.blend = 0.5f;

            mat.bumpMap = new CS123SceneFileMap();
            mat.bumpMap->filename = "";
            mat.bumpMap->isUsed = false;
            mat.bumpMap->repeatU = 1;
            mat.bumpMap->repeatV = 1;
            mat.bumpMap->texid = 0;

            SceneElement *element = new SceneElement();
            element->dragged = false;
            element->render = true;
            element->linked = false;
            element->link = -1;
            element->port = m_unused.takeLast();

            switch (element->port) {
            case 7005:
                m_udp5->sendAddition(5);
                break;
            case 7006:
                m_udp6->sendAddition(6);
                break;
            case 7007:
                m_udp7->sendAddition(7);
                break;
            case 7008:
                m_udp8->sendAddition(8);
                break;
            }

            prim->type = WATER_TYPE;
//            element->cube = new CubeMap();
//            element->cube->makeCubeMap(m_images);
            element->primitive = prim;
            element->trans = glm::mat4(1.0f);
            element->inv = glm::inverse(element->trans);
            m_waterElements.append(element);
        }
    }
    else if (typ == LIGHTNING_TYPE) {
        if (m_lightningElements.size() >= 15) {
            return;
        }
        else {
            CS123ScenePrimitive *prim = new CS123ScenePrimitive();
            CS123SceneMaterial& mat = prim->material;

            prim->meshfile = "";
          //  prim->type = PRIMITIVE_CUBE;

            // Use a shiny orange material
            memset(&mat, 0, sizeof(CS123SceneMaterial));
            mat.cAmbient.r = 0.2f;
            mat.cAmbient.g = 0.2f;
            mat.cAmbient.b = 0.4f;
            mat.cDiffuse.r = 0.5f;
            mat.cDiffuse.g = 0.5f;
            mat.cDiffuse.b = 1.0f;
            mat.cSpecular.r = mat.cSpecular.g = mat.cSpecular.b = 1;
            mat.shininess = 64;

            // Use snow texture
            mat.textureMap = new CS123SceneFileMap();
        //    mat.textureMap->filename = "/course/cs123/data/image/terrain/snow.JPG";
            mat.textureMap->filename = "/Users/Logan/Documents/course/cs123/data/image/terrain/snow.JPG";
            mat.textureMap->isUsed = false;
            mat.textureMap->repeatU = 1;
            mat.textureMap->repeatV = 1;
            mat.blend = 0.5f;

            mat.bumpMap = new CS123SceneFileMap();
            mat.bumpMap->filename = "";
            mat.bumpMap->isUsed = false;
            mat.bumpMap->repeatU = 1;
            mat.bumpMap->repeatV = 1;
            mat.bumpMap->texid = 0;

            SceneElement *element = new SceneElement();
            element->dragged = false;
            element->render = true;
            element->linked = false;
            element->link = -1;

            prim->type = LIGHTNING_TYPE;
            element->primitive = prim;
            element->trans = glm::mat4(1.0f);
            element->inv = glm::inverse(element->trans);
            m_lightningElements.append(element);
        }
    }
}


void Scene::renderFilter(GLuint shader)
{
//    applyMaterial(m_filterMaterial, shader);
//    m_filter->transformAndRender();

}


void Scene::renderLightning(GLuint shader)
{

    if (!m_initialized)
        return;

//    applyMaterial(m_lightningElements.at(0)->primitive->material, shader);

////     Draw the grid. CAN'T DRAW WITH CURRENT GEOMETRY SHADER SET TO TRIANGLES
//    glUniform1i(glGetUniformLocation(shader, "functionSize"), 0);
//    glUniform3f(glGetUniformLocation(shader, "allWhite"), 1, 1, 1); // make white
//    m_grid->transformAndRender(shader, glm::mat4());


    // Draw the shapes.
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        if (m_lightningElements.at(i)->render) {
            glm::vec3 cent = glm::vec3((m_lightningElements.at(i)->trans[3]));
            glUniform3fv(glGetUniformLocation(shader, "centr"), 1, glm::value_ptr(cent));
            glUniform3f(glGetUniformLocation(shader, "allWhite"), 0, 0, 0); // not white
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f1.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f1.size(), m_f1.data());
            m_lightningShape->transformAndRender(shader, m_lightningElements.at(i)->trans);
        }

    }
}

void Scene::renderBolts(GLuint shader)
{
    QVector<float> f1 = QVector<float>(m_f1);
    f1.clear();
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        if (m_lightningElements.at(i)->render) {
            m_lightningShape->calcBoltVerts(f1);
            m_lightningShape->updateLightning(shader);
            m_lightningShape->renderLightning(shader, m_lightningElements.at(i)->trans);
        }
    }

    for (int i = 0; i < m_waterElements.size(); ++i) {
        if (m_waterElements.at(i)->linked) {
            m_waterShape->calcBoltVerts(f1);
            m_waterShape->updateLightning(shader);
            m_waterShape->renderLightning(shader, m_waterElements.at(i)->trans);
        }

    }

//    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f3.size());
//    glUniform1fv(glGetUniformLocation(shader, "function"), m_f3.size(), m_f3.data());
//    m_lightningShape->transformAndRender(shader, glm::translate(glm::vec3(-2.f, 1.f, 0.f)));

//    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f4.size());
//    glUniform1fv(glGetUniformLocation(shader, "function"), m_f4.size(), m_f4.data());
//    m_lightningShape->transformAndRender(shader, glm::translate(glm::vec3(2.f, -1.f, 0.f)));
}


void Scene::renderTransparents(GLuint shader)
{
    glUniform1f(glGetUniformLocation(shader, "r0"), 0.9f);
    glUniform3f(glGetUniformLocation(shader, "eta"), 1.f / 1.3312f, 1.f / 1.333f, 1.f / 1.3381f); // water
    glUniform1f(glGetUniformLocation(shader, "m"), 0.02f);
    glUniform4fv(glGetUniformLocation(shader, "lightPosition"), 1, glm::value_ptr(m_lights.value(0)->pos));

//    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f1.size());
//    glUniform1fv(glGetUniformLocation(shader, "function"), m_f1.size(), m_f1.data());
//    m_waterShape->transformAndRender(shader, m_elements.at(0)->trans);
//    m_waterShape->transformAndRender(shader, glm::translate(glm::vec3(0, 0, 2)));

    SceneElement *element;
    for (int i = 0; i < m_waterElements.size(); ++i) {

        element = m_waterElements.value(i);

        glActiveTexture(GL_TEXTURE1);
        if (element->linked) {
            m_cm->bindTexture();
            glUniform3f(glGetUniformLocation(shader, "whiterer"), .1f, .1f, .1f);
        } else {
            m_room->bindTexture();
            glUniform3f(glGetUniformLocation(shader, "whiterer"), 0.f, 0.f, 0.f);
        }
        glActiveTexture(GL_TEXTURE0);
//        m_waterElements.value(i)->cube->bindTexture();

        QVector<float> temp = QVector<float>();
        for (int k = 0; k < 25; k++)
            if (k < 5)
                temp.append(0.0);
            else
                temp.append(1.0);

//        cout << temp.size() << endl;

        switch (element->port) {
        case 7001:
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f1.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f1.size(), m_f1.data());
            break;
        case 7002:
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f2.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f2.size(), m_f2.data());
            break;
        case 7005:
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f5.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f5.size(), m_f5.data());
            break;
        case 7006:
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f6.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f6.size(), m_f6.data());
            break;
        case 7007:
            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f7.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), m_f7.size(), m_f7.data());
            break;
        case 7008:
//            glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f8.size());
//            glUniform1fv(glGetUniformLocation(shader, "function"), m_f8.size(), m_f8.data());
            glUniform1i(glGetUniformLocation(shader, "functionSize"), temp.size());
            glUniform1fv(glGetUniformLocation(shader, "function"), temp.size(), temp.data());
            break;
        default:
            break;
        }
        m_waterShape->transformAndRender(shader, element->trans);
    }
    m_room->bindTexture();
    glUniform3f(glGetUniformLocation(shader, "whiterer"), 0.f, 0.f, 0.f);
    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f3.size());
    glUniform1fv(glGetUniformLocation(shader, "function"), m_f3.size(), m_f3.data());
    m_waterShape->transformAndRender(shader, m_input->trans);

}


void Scene::setLights(const glm::mat4 viewMatrix, GLuint shader)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    CS123SceneLightData &light = *(m_lights.at(0));
//    light.dir = /*glm::inverse(viewMatrix) **/ lightDirection;

    clearLights(shader);
    setLight(light, shader);
}

void Scene::checkAsserts(bool durClick) {
    bool beingDragged = false;
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        assert(m_lightningElements.at(i)->linked == false);
        assert(m_lightningElements.at(i)->link == -1);
        if (m_lightningElements.at(i)->dragged) {
            assert(beingDragged == false);
            beingDragged = true;
        }
    }
    for (int i = 0; i < m_waterElements.size(); ++i) {
        if (m_waterElements.at(i)->linked) {
            int lInd = m_waterElements.at(i)->link;
//            std::cout << " LIGHNING IND " << lInd << " WATER IND " << i << " LIGHT SIZE " << m_lightningElements.size() << " W SIZE " << m_waterElements.size() << std::endl;
            assert(lInd < m_lightningElements.size());
            assert(m_lightningElements.at(lInd)->render == false);
            assert(lInd >= 0);
        }
        else {
            assert(m_waterElements.at(i)->link == -1);
        }
        if (m_waterElements.at(i)->dragged) {
            assert(beingDragged == false);
            beingDragged = true;
        }
    }
    if (durClick) {
        assert(beingDragged == true);
    }
    else {
        assert(beingDragged == false);
    }
}

void Scene::updateShape(int ind, float x, float y, float z, PrimitiveType prm) {
    //std::cout << " ind  " << ind << " X " << x << " Y " << y << std::endl;
    glm::mat4x4 transla = glm::mat4x4(1.0f, 0.0f, 0.0f, x,
                                     0.0f, 1.0f, 0.0f, y,
                                     0.0f, 0.0f, 1.0f, z,
                                    0.0f, 0.0f, 0.0f, 1.0f);
    if (prm == WATER_TYPE) {
        glm::mat4x4 transfor = m_waterElements.at(ind)->trans;
        m_waterElements.at(ind)->trans = (glm::transpose(transla) * transfor);
        //m_waterElements.at(ind)->trans = (glm::translate(glm::mat4(1.0f), glm::vec3(( 1.3f * x), (1.3f * y), z)) * m_waterElements.at(ind)->trans);
        m_waterElements.at(ind)->inv = glm::inverse(m_waterElements.at(ind)->trans);
        if (m_waterElements.at(ind)->linked) {
            int lInd = m_waterElements.at(ind)->link;
            m_lightningElements.at(lInd)->trans = m_waterElements.at(ind)->trans;
            m_lightningElements.at(lInd)->inv = m_waterElements.at(ind)->inv;
        }
    }
    else if (prm == LIGHTNING_TYPE) {
        glm::mat4x4 transfor = m_lightningElements.at(ind)->trans;
        m_lightningElements.at(ind)->trans = (glm::transpose(transla) * transfor);
        //m_lightningElements.at(ind)->trans = (glm::translate(glm::mat4(1.0f), glm::vec3(( 1.3f * x), (1.3f * y), z)) * m_lightningElements.at(ind)->trans);
        m_lightningElements.at(ind)->inv = glm::inverse(m_lightningElements.at(ind)->trans);
    }


}

void Scene::checkIntersects() {

    float dist;
    for (int w = 0; w < m_waterElements.size(); ++w) {
        glm::vec3 pos1 = glm::vec3(m_waterElements.at(w)->trans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        float rad1 = m_waterShape->getRadius();

        for (int l = 0; l < m_lightningElements.size(); ++l) {
            if (m_lightningElements.at(l)->render) {
                glm::vec3 pos2 = glm::vec3(m_lightningElements.at(l)->trans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                float rad2 = m_lightningShape->getRadius();
                if ((dist = m_waterShape->collisionDetector(pos1, pos2, rad1, rad2)) != 0.0f) {
                    if (m_waterElements.at(w)->linked) {
                        glm::vec3 dPos = (pos2 - pos1);
                        if (glm::dot(dPos, dPos) == 0.0f) {
                            dPos = glm::vec3(1.0f, 0.0f, 0.0f);
                        }
                        glm::vec3 dir = glm::normalize(dPos);
                        glm::vec3 trans = ((dist/2.0f) * dir);
                        glm::vec3 trans2 = (dist * dir);
                        if (m_waterElements.at(w)->dragged) {
                            updateShape(l, trans2.x, trans2.y, trans2.z, LIGHTNING_TYPE);
                        }
                        else if (m_lightningElements.at(l)->dragged) {
                            updateShape(w, -trans2.x, -trans2.y, -trans2.z, WATER_TYPE);
                        }
                        else {
                            updateShape(w, -trans.x, -trans.y, -trans.z, WATER_TYPE);
                            updateShape(l, trans.x, trans.y, trans.z, LIGHTNING_TYPE);
                        }
                    }
                    else {

                        m_lightningElements.at(l)->trans = m_waterElements.at(w)->trans;
                        m_lightningElements.at(l)->inv = m_waterElements.at(w)->inv;
                        m_lightningElements.at(l)->render = false;
                        m_waterElements.at(w)->link = l;
                        m_waterElements.at(w)->linked = true;
                    }
                }
            }
        }

        for (int wi = 0; wi < m_waterElements.size(); ++wi) {
            if (wi != w) {
                glm::vec3 pos2 = glm::vec3(m_waterElements.at(wi)->trans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                float rad2 = m_waterShape->getRadius();
                if ((dist = m_waterShape->collisionDetector(pos1, pos2, rad1, rad2)) != 0.0f) {
                    glm::vec3 dPos = (pos2 - pos1);
                    if (glm::dot(dPos, dPos) == 0.0f) {
                        dPos = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    glm::vec3 dir = glm::normalize(dPos);
                    glm::vec3 trans = ((dist/2.0f) * dir);
                    glm::vec3 trans2 = (dist * dir);
                    if (m_waterElements.at(w)->linked) {
                        if (!m_waterElements.at(wi)->linked) {
                            int lInd2;
                            if ((lInd2 = m_lightningElements.size()) < 15) {

                                addObject(LIGHTNING_TYPE);
                                m_lightningElements.at(lInd2)->trans = m_waterElements.at(wi)->trans;
                                m_lightningElements.at(lInd2)->inv = m_waterElements.at(wi)->inv;
                                m_lightningElements.at(lInd2)->render = false;
                                m_waterElements.at(wi)->link = lInd2;
                                m_waterElements.at(wi)->linked = true;
                            }
                        }
                    }
                    else {
                        if (m_waterElements.at(wi)->linked) {
                            int lInd2;
                            if ((lInd2 = m_lightningElements.size()) < 15) {

                                addObject(LIGHTNING_TYPE);
                                m_lightningElements.at(lInd2)->trans = m_waterElements.at(w)->trans;
                                m_lightningElements.at(lInd2)->inv = m_waterElements.at(w)->inv;
                                m_lightningElements.at(lInd2)->render = false;
                                m_waterElements.at(w)->link = lInd2;
                                m_waterElements.at(w)->linked = true;
                            }
                        }
                    }

                    if (m_waterElements.at(w)->dragged) {
                        updateShape(wi, trans2.x, trans2.y, trans2.z, WATER_TYPE);
                    }
                    else if (m_waterElements.at(wi)->dragged) {
                        updateShape(w, -trans2.x, -trans2.y, -trans2.z, WATER_TYPE);
                    }
                    else {
                        updateShape(w, -trans.x, -trans.y, -trans.z, WATER_TYPE);
                        updateShape(wi, trans.x, trans.y, trans.z, WATER_TYPE);
                    }

                }
            }
        }
    }


    for (int l = 0; l < m_lightningElements.size(); ++l) {
        glm::vec3 pos1 = glm::vec3(m_lightningElements.at(l)->trans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        float rad1 = m_lightningShape->getRadius();

        for (int li = 0; li < m_lightningElements.size(); ++li) {
            if (li != l) {
                glm::vec3 pos2 = glm::vec3(m_lightningElements.at(li)->trans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                float rad2 = m_lightningShape->getRadius();
                if ((dist = m_lightningShape->collisionDetector(pos1, pos2, rad1, rad2)) != 0.0f) {

                    glm::vec3 dPos = (pos2 - pos1);
                    if (glm::dot(dPos, dPos) == 0.0f) {
                        dPos = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    glm::vec3 dir = glm::normalize(dPos);
                    glm::vec3 trans = ((dist/2.0f) * dir);
                    glm::vec3 trans2 = (dist * dir);

                    if (m_lightningElements.at(l)->dragged) {
                        updateShape(li, trans2.x, trans2.y, trans2.z, LIGHTNING_TYPE);
                    }
                    else if (m_lightningElements.at(li)->dragged) {
                        updateShape(l, -trans2.x, -trans2.y, -trans2.z, LIGHTNING_TYPE);
                    }
                    else {
                        updateShape(l, -trans.x, -trans.y, -trans.z, LIGHTNING_TYPE);
                        updateShape(li, trans.x, trans.y, trans.z, LIGHTNING_TYPE);
                    }

                }
            }
        }
    }
}

IntersectElement Scene::shapeClickIntersect(glm::vec4 ey, glm::vec4 dr) {
    float finDist = -1.0f;
    int finInd = -1;
    PrimitiveType prm;
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        if (m_lightningElements.at(i)->render) {
            glm::vec4 eye = m_lightningElements.at(i)->inv * ey;
            glm::vec4 dir = m_lightningElements.at(i)->inv * dr;

            glm::vec3 eye3 = glm::vec3(eye);
            glm::vec3 dir3 = glm::vec3(dir);



            float tempDist = m_lightningShape->calcIntersect(eye3, dir3);

            if (tempDist > 0.0f) {

                if(finDist < 0.0f) {
                    finDist = tempDist;
                    finInd = i;
                    prm = LIGHTNING_TYPE;
                }
                else if (tempDist < finDist) {
                    finDist = tempDist;
                    finInd = i;
                    prm = LIGHTNING_TYPE;
                }
            }
        }
    }
    for (int i = 0; i < m_waterElements.size(); ++i) {
        glm::vec4 eye = m_waterElements.at(i)->inv * ey;
        glm::vec4 dir = m_waterElements.at(i)->inv * dr;

        glm::vec3 eye3 = glm::vec3(eye);
        glm::vec3 dir3 = glm::vec3(dir);



        float tempDist = m_waterShape->calcIntersect(eye3, dir3);

        
        if (tempDist > 0.0f) {

            if (prm == LIGHTNING_TYPE) {
                if (m_waterElements.at(i)->linked) {
                    int lInd = m_waterElements.at(i)->link;
                    if ((finInd == lInd) && !m_lightningElements.at(lInd)->render) {
                        finInd = i;
                        finDist = tempDist;
                        prm = WATER_TYPE;
                    }
                }
            }
            
            if(finDist < 0.0f) {
                finDist = tempDist;
                finInd = i;
                prm = WATER_TYPE;
            }
            else if (tempDist < finDist) {
                finDist = tempDist;
                finInd = i;
                prm = WATER_TYPE;
            }
        }
    }


    IntersectElement intrRet = IntersectElement();
    intrRet.indx = finInd;
    intrRet.interT = finDist;
    intrRet.mHit = (ey + (dr * finDist));
    intrRet.prim = prm;
    return intrRet;
}


//void Scene::checkFilters()
//{

////    cout << "size: " << m_waterElements.size() << endl;

//    glm::vec3 wpos, lpos;
//    float dist;
//    foreach (SceneElement *we, m_waterElements)
//    {
////        cout << "we: " << glm::to_string(we->trans[3]) << endl;
//        wpos = glm::vec3(we->trans[3]);
//        foreach (SceneElement *le, m_lightningElements)
//        {
////            cout << "le: " << glm::to_string(le->trans[3]) << endl;
//            lpos = glm::vec3(le->trans[3]);
//            dist = glm::distance(wpos, lpos);
////            cout << dist << endl;
//            if (dist > 0.65f)
//                continue;
//            else
//                cout << "blah" << endl;
//            if (lpos.y - 0.15f > wpos.y + .5f) {
//                cout << lpos.y - 0.15f << ", " << wpos.y + .5f << endl;
//                continue;
//            } else {
//                cout << "blah2" << endl;
//            }
//            if (lpos.y + 0.15f < wpos.y - 0.5f)
//                continue;
//            else
//                cout << "blah3" << endl;

//            switch(we->port)
//            {
//            case 7001:
//                m_udp1->sendFilter(.25f, .01f);
//                break;
//            case 7002:
//                m_udp2->sendFilter(.25f, .5f);
//                break;
//            case 7005:
//                m_udp5->sendFilter(.25f, .5f);
//                break;
//            case 7006:
//                m_udp6->sendFilter(.25f, .5f);
//                break;
//            case 7007:
//                m_udp7->sendFilter(.25f, .5f);
//                break;
//            case 7008:
//                m_udp8->sendFilter(.25f, .5f);
//                break;
//            default:
//                break;
//            }


//        }
//    }
//}


