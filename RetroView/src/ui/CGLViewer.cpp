#include "core/GLInclude.hpp"
#include <Athena/Exception.hpp>
#include <QImage>
#include <QSettings>
#include <QThread>
#include <QFileInfo>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QStatusBar>

#include <glm/gtc/matrix_transform.hpp>

#include "core/CResourceManager.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "core/IRenderableModel.hpp"
#include "ui/CGLViewer.hpp"

CGLViewer* CGLViewer::m_instance = NULL;
CGLViewer::CGLViewer(QWidget* parent)
    : QOpenGLWidget(parent),
      m_currentRenderable(nullptr),
      m_skybox(nullptr),
      m_camera(glm::vec3(0.0f, 10.0f, 3.0f)),
      m_mouseEnabled(false),
      m_isInitialized(false)
{
    QOpenGLWidget::setMouseTracking(true);
    m_instance = this;
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start(1);

    // Set our scene bounds
    m_sceneBounds.min = glm::vec3(-100.f);
    m_sceneBounds.max = glm::vec3( 100.f);
    
}

CGLViewer::~CGLViewer()
{
    m_updateTimer.stop();
    std::cout << "I'M DYING!!!" << std::endl;
}

// TODO: Clean this up
void CGLViewer::paintGL()
{
    m_currentTime = 1.f * hiresTimeMS();
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;
    
    updateCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glm::mat4 modelMat = modelMatrix();
    glm::mat4 viewMat = viewMatrix();
    glm::mat4 projectionMat = projectionMatrix();
    glm::mat4 mvp = projectionMat * viewMat * modelMat;
#if 0
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(&mvp[0][0]);

    bool _drawAxis = QSettings().value("axisDrawn").toBool();
    bool _drawGrid = QSettings().value("gridDrawn").toBool();

    glEnable( GL_LINE_SMOOTH );
    glLineWidth( 1.5 );

    if (_drawAxis)
        drawAxis(glm::vec3(0), glm::vec3(0), 20.f, false);

    glColor3ub(128, 128, 128);
    if (_drawGrid)
    {
        glBegin(GL_LINES);
        for(int i = -20; i <= 20; i += 1)
        {
            if (i == 0 && _drawAxis)
                continue;

            glVertex3f((float)i, -20.0f, 0.0f);
            glVertex3f((float)i, 20.0f, 0.0f);
            glVertex3f(-20.0f, (float)i, 0.0f);
            glVertex3f( 20.0f, (float)i, 0.0f);
        }
        if (_drawAxis)
        {
            glColor3ub(128, 128, 128);
            glVertex3f(  0,   0,   0);
            glColor3ub(128, 128, 128);
            glVertex3f(-20,   0,   0);
            glColor3ub(128, 128, 128);
            glVertex3f(  0,  0,    0);
            glColor3ub(128, 128, 128);
            glVertex3f(  0, -20,   0);
        }
        glEnd();
    }

    glLineWidth( 1 );

    glDisable(GL_LINE_SMOOTH);
#endif
    if (m_currentRenderable)
    {
        m_currentRenderable->updateViewProjectionUniforms(viewMat, projectionMat);
        bool texturesEnabled = (QSettings().value("enableTextures").toBool() && !QSettings().value("drawPoints").toBool() && !QSettings().value("wireframe").toBool());
        m_currentRenderable->updateTexturesEnabled(texturesEnabled);
        m_currentRenderable->draw();
    }
    
}

void CGLViewer::resizeGL(int w, int h)
{
    m_camera.setViewport(w, h);
    glViewport(0, 0, w, h);
    QOpenGLWidget::resize(w, h);
}

void CGLViewer::initializeGL()
{
    if (!m_isInitialized)
    {
        QOpenGLWidget::initializeGL();

#ifndef __APPLE__
        glewExperimental = GL_TRUE;
        glewInit();
#endif
        CMaterialCache::instance()->initialize();

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        QColor clearColor = QColor(32, 32, 32);
        glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0);
        emit initialized();
        
    }

    m_frameTimer.start();
}

void CGLViewer::closeEvent(QCloseEvent* ce)
{
    emit closing();
    QOpenGLWidget::closeEvent(ce);
}

void CGLViewer::mouseMoveEvent(QMouseEvent* e)
{
    static QPoint lastPos = e->pos();
    QPoint curPos = e->pos();

    float xoffset = curPos.x() - lastPos.x();
    float yoffset = lastPos.y() - curPos.y();

    lastPos = curPos;

    if (m_mouseEnabled)
    {
        m_camera.processMouseMovement(xoffset, yoffset);

//        QPoint newPos = curPos;
//        if (curPos.x() <= 0)
//            newPos.setX(size().width());
//        else if (curPos.x() >= size().width())
//            newPos.setX(1);

//        if (newPos != curPos)
//        {
//            cursor().setPos(mapToGlobal(newPos));
//            lastPos = newPos;
//        }
    }
    QOpenGLWidget::mouseMoveEvent(e);
}

void CGLViewer::mousePressEvent(QMouseEvent* e)
{
    m_mouseEnabled = true;
    QOpenGLWidget::mousePressEvent(e);
    cursor().setShape(Qt::BlankCursor);
}

void CGLViewer::mouseReleaseEvent(QMouseEvent* e)
{
    m_mouseEnabled = false;
    QOpenGLWidget::mousePressEvent(e);
    cursor().setShape(Qt::ArrowCursor);
}

void CGLViewer::wheelEvent(QWheelEvent* e)
{
    if (e->delta() > 0)
        m_camera.increaseSpeed();
    else
        m_camera.decreaseSpeed();

    QOpenGLWidget::wheelEvent(e);
}

void CGLViewer::updateCamera()
{
    CKeyboardManager* km = CKeyboardManager::instance();
    if (km->isKeyPressed(Qt::Key_W))
        m_camera.processKeyboard(CCamera::FORWARD, 1.f);
    if (km->isKeyPressed(Qt::Key_S))
        m_camera.processKeyboard(CCamera::BACKWARD, 1.f);
    if (km->isKeyPressed(Qt::Key_A))
        m_camera.processKeyboard(CCamera::LEFT, 1.f);
    if (km->isKeyPressed(Qt::Key_D))
        m_camera.processKeyboard(CCamera::RIGHT, 1.f);
}

glm::mat4 CGLViewer::projectionMatrix()
{
    return m_camera.projectionMatrix();
}

glm::mat4 CGLViewer::modelMatrix()
{
    return glm::mat4(1);
}

glm::mat4 CGLViewer::viewMatrix()
{
    return m_camera.viewMatrix();
}

void CGLViewer::setCurrent(IRenderableModel* renderable)
{
    m_currentRenderable = renderable;
}

void CGLViewer::setSkybox(IRenderableModel* renderable)
{
    m_skybox = renderable;
}

float CGLViewer::frameRate() const
{
    return 1000.f / m_deltaTime;
}

IRenderableModel* CGLViewer::currentModel()
{
    return m_currentRenderable;
}

void CGLViewer::stopUpdates()
{
    m_updateTimer.stop();
}

void CGLViewer::startUpdates()
{
    m_updateTimer.start();
}

void CGLViewer::exportFile()
{
    if (!m_currentRenderable)
        return;

    QString exportPath = QFileDialog::getSaveFileName(this, "Export Model...", QString(), "Wavefron Obj *.obj (*.obj)");
    if (!exportPath.isEmpty())
    {
        if (!exportPath.contains(".obj"))
            exportPath += ".obj";

        m_currentRenderable->exportToObj(exportPath.toStdString());
    }
}

CGLViewer* CGLViewer::instance()
{
    return m_instance;
}

void CGLViewer::resetCamera()
{
    m_camera.setPitch(0.f);
    m_camera.setYaw(-90.f);
    m_camera.setPosition(glm::vec3(0.0f, 10.0f, 3.0f));
    update();
}

void CGLViewer::setAxisIsDrawn(bool drawn)
{
    QSettings().setValue("axisDrawn", drawn);
}

void CGLViewer::setGridIsDrawn(bool drawn)
{
    QSettings().setValue("gridDrawn", drawn);
}
