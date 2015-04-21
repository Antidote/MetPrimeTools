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

#include "models/CAreaFile.hpp"
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
      m_camera(CVector3f(0.0f, 10.0f, 3.0f)),
      m_buttons(Qt::NoButton),
      m_isInitialized(false),
      m_skyVisible(false),
      m_isFlyMode(false)
{
    QOpenGLWidget::setMouseTracking(true);
    m_instance = this;
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start(1);
}

CGLViewer::~CGLViewer()
{
    QSettings().setValue("movementSpeed", m_camera.movementSpeed());
    m_updateTimer.stop();
    std::cout << "I'M DYING!!!" << std::endl;
}

// TODO: Clean this up
void CGLViewer::drawSky()
{
    if (m_skybox)
    {
        //glDepthRangef(0.0090001f, 1.0f);
        CTransform viewRot = m_camera.rotation();
        m_skybox->setAmbient(1, 1, 1);
        m_skybox->updateViewProjectionUniforms(viewRot, m_camera.projection());
        m_skybox->updateTexturesEnabled(QSettings().value("enableTextures").toBool());
        m_skybox->draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void CGLViewer::setSkyVisible(bool visible)
{
    m_skyVisible = visible;
}

float CGLViewer::movementSpeed() const
{
    return m_camera.movementSpeed();
}

void CGLViewer::setMovementSpeed(float speed)
{
    m_camera.setMovementSpeed(speed);
    emit movementSpeedChanged(m_camera.movementSpeed());
}

void CGLViewer::paintGL()
{
    m_currentTime = 1.f * hiresTimeMS();
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;

    updateCamera(m_deltaTime);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

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

    //drawSky();
    //drawOutlinedCube(CVector3f(), CColor(), CColor());

    if (m_currentRenderable)
    {
        CAreaFile* area = dynamic_cast<CAreaFile*>(m_currentRenderable);
        if (area && area->skyEnabled())
            drawSky();

        m_currentRenderable->updateViewProjectionUniforms(view(), projection());
        bool texturesEnabled = (QSettings().value("enableTextures").toBool() && !QSettings().value("drawPoints").toBool() && !QSettings().value("wireframe").toBool());
        m_currentRenderable->updateTexturesEnabled(texturesEnabled);

        if (QSettings().value("drawBoundingBox").toBool())
            m_currentRenderable->drawBoundingBox();

        if (QSettings().value("wireframe").toBool() && !QSettings().value("drawPoints").toBool())
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else if (QSettings().value("drawPoints").toBool())
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


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
        setMovementSpeed(QSettings().value("movementSpeed", 0.5f).toFloat());
        emit initialized();        
    }

    m_frameTimer.start();
}

void CGLViewer::closeEvent(QCloseEvent* ce)
{
    emit closing();
    QOpenGLWidget::closeEvent(ce);
}

void CGLViewer::wheelEvent(QWheelEvent* e)
{
    if (e->delta() < 0)
        m_camera.changeSpeed(-1.f);
    else
        m_camera.changeSpeed(1.f);

    emit movementSpeedChanged(m_camera.movementSpeed());
    QOpenGLWidget::wheelEvent(e);
}

void CGLViewer::updateCamera(float delta)
{
    CKeyboardManager* km = CKeyboardManager::instance();
    if (km->isKeyPressed(Qt::Key_W))
        m_camera.processKeyboard(CCamera::FORWARD, delta);
    if (km->isKeyPressed(Qt::Key_S))
        m_camera.processKeyboard(CCamera::BACKWARD, delta);
    if (km->isKeyPressed(Qt::Key_A))
        m_camera.processKeyboard(CCamera::LEFT, delta);
    if (km->isKeyPressed(Qt::Key_D))
        m_camera.processKeyboard(CCamera::RIGHT, delta);
    if (km->isKeyPressed(Qt::Key_Q))
        m_camera.processKeyboard(CCamera::UP, delta);
    if (km->isKeyPressed(Qt::Key_E))
        m_camera.processKeyboard(CCamera::DOWN, delta);
    if (km->isKeyPressed(Qt::Key_Shift))
    {
        if (km->isKeyPressed(Qt::Key_F))
        {
            m_isFlyMode = true;
            qApp->setOverrideCursor(Qt::BlankCursor);
        }
    }
    if (km->isKeyPressed(Qt::Key_Escape))
    {
        m_isFlyMode = false;
    }

    if (!m_isFlyMode)
        qApp->setOverrideCursor(Qt::ArrowCursor);

    m_buttons = qApp->mouseButtons();
    static QPoint lastPos = QCursor::pos();
    QPoint curPos = QCursor::pos();

    if ((m_buttons != Qt::NoButton && this->hasFocus()) || m_isFlyMode)
    {
        float xoffset = curPos.x() - lastPos.x();
        float yoffset = lastPos.y() - curPos.y();


        if (m_buttons & Qt::LeftButton || m_isFlyMode)
            m_camera.processMouseMovement(xoffset, yoffset);
        else if (m_buttons & Qt::MiddleButton)
            m_camera.processMouseDolly(yoffset);
        else if (m_buttons & Qt::RightButton)
            m_camera.processMouseStrafe(xoffset);

        curPos = mapFromGlobal(curPos);
        QPoint newPos = curPos;
        if (curPos.x() <= 4)
            newPos.setX(size().width() - 5);
        else if (curPos.x() >= size().width() - 4)
            newPos.setX(5);
        if (curPos.y() <= 4)
            newPos.setY(size().height() - 4);
        else if (curPos.y() >= size().height() - 5)
            newPos.setY(5);

        if (newPos != curPos)
        {
            cursor().setPos(mapToGlobal(newPos));
            curPos = newPos;
        }
        curPos = mapToGlobal(curPos);
    }

    if (m_buttons != Qt::NoButton && m_isFlyMode)
    {
        curPos = mapToGlobal(QPoint(width()/2, height()/2));
        cursor().setPos(curPos);
        m_isFlyMode = false;
    }

    lastPos = curPos;
}

CProjection CGLViewer::projection()
{
    return m_camera.projection();
}

CTransform CGLViewer::model()
{
    return CTransform();
}

CTransform CGLViewer::view()
{
    return m_camera.view();
}

CVector3f CGLViewer::cameraPosition()
{
    return m_camera.position();
}

CVector3f CGLViewer::cameraVector()
{
    return m_camera.front();
}

CQuaternion CGLViewer::cameraRotation()
{
    return m_camera.rotationAsQuaternion();
}

void CGLViewer::setCurrent(IRenderableModel* renderable)
{
    m_currentRenderable = renderable;
    CAreaFile* area = dynamic_cast<CAreaFile*>(m_currentRenderable);
    if (area)
    {
        CVector3f pos;
        CVector3f rot;
        area->nearestSpawnPoint(m_camera.position(), pos, rot);
        m_camera.setPosition(pos);
        m_camera.setRotation(rot);
    }
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
    m_camera.setPosition(CVector3f(0.0f, 10.0f, 3.0f));
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
