#include <GL/glew.h>
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
#include <Athena/BinaryWriter.hpp>

#include "CResourceManager.hpp"
#include "CMaterialCache.hpp"
#include "CGLViewer.hpp"
#include "IRenderableModel.hpp"
#include "GXCommon.hpp"

//TODO: Remove this
#include "CAreaFile.hpp"

CGLViewer* CGLViewer::m_instance = NULL;
CGLViewer::CGLViewer(QWidget* parent)
    : QGLWidget(parent),
      m_currentRenderable(nullptr),
      m_camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      m_mouseEnabled(false),
      m_isInitialized(false)
{
    QGLWidget::setMouseTracking(true);
    m_instance = this;
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start(0);

    // Set our scene bounds
    m_sceneBounds.min = glm::vec3(-100.f);
    m_sceneBounds.max = glm::vec3( 100.f);
}

CGLViewer::~CGLViewer()
{
    m_updateTimer.stop();
    std::cout << "I'M DYING!!!" << std::endl;
}

void CGLViewer::paintGL()
{
    QGLWidget::paintGL();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 modelMat = modelMatrix();
    glm::mat4 viewMat = viewMatrix();
    glm::mat4 projectionMat = projectionMatrix();
    glm::mat4 mvp = projectionMat * viewMat * modelMat;
    glLoadMatrixf(&mvp[0][0]);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_COLOR_MATERIAL);
    glPrimitiveRestartIndex(0xFFFFFFFF);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);

    if (QSettings().value("drawTextures").toBool())
        glEnable(GL_FRAGMENT_PROGRAM_ARB);
    else
        glDisable(GL_FRAGMENT_PROGRAM_ARB);

    updateCamera();

    if (QSettings().value("enableLighting").toBool())
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

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
        for(int i = -20; i <= 20; i += 2)
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


    if (m_currentRenderable)
    {
        m_currentRenderable->updateViewProjectionUniforms(viewMat, projectionMat);

        if (QSettings().value("drawBoundingBox").toBool())
            m_currentRenderable->drawBoundingBox();

        if (QSettings().value("wireframe").toBool())
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_currentRenderable->draw();
    }

}

void CGLViewer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    QGLWidget::resize(w, h);
}

void CGLViewer::initializeGL()
{
    QGLWidget::initializeGL();

    if (!m_isInitialized)
    {
        m_isInitialized = true;
        glewExperimental = true;
        glewInit();
        CMaterialCache::instance()->initialize();


        qglClearColor(QColor(128, 128, 128).darker(300));
        emit initialized();
    }

    m_frameTimer.start();
}

void CGLViewer::closeEvent(QCloseEvent* ce)
{
    emit closing();
    QGLWidget::closeEvent(ce);
}

void CGLViewer::mouseMoveEvent(QMouseEvent* e)
{
    static QPoint lastPos = e->pos();

    float xoffset = e->pos().x() - lastPos.x();
    float yoffset = lastPos.y() - e->pos().y();

    lastPos = e->pos();

    if (m_mouseEnabled)
        m_camera.processMouseMovement(xoffset, yoffset);
    QGLWidget::mouseMoveEvent(e);
}

void CGLViewer::mousePressEvent(QMouseEvent* e)
{
    m_mouseEnabled = true;
    QGLWidget::mousePressEvent(e);
    cursor().setShape(Qt::BlankCursor);
}

void CGLViewer::mouseReleaseEvent(QMouseEvent* e)
{
    m_mouseEnabled = false;
    QGLWidget::mousePressEvent(e);
    cursor().setShape(Qt::ArrowCursor);
}

void CGLViewer::wheelEvent(QWheelEvent* e)
{
    if (e->delta() > 0)
        m_camera.increaseSpeed();
    else
        m_camera.decreaseSpeed();

    QGLWidget::wheelEvent(e);
}

void CGLViewer::updateCamera()
{
    CKeyboardManager* km = CKeyboardManager::instance();
    if (km->isKeyPressed(Qt::Key_W))
        m_camera.processKeyboard(CCamera::FORWARD, 1.0f);
    if (km->isKeyPressed(Qt::Key_S))
        m_camera.processKeyboard(CCamera::BACKWARD, 1.0f);
    if (km->isKeyPressed(Qt::Key_A))
        m_camera.processKeyboard(CCamera::LEFT, 1.0f);
    if (km->isKeyPressed(Qt::Key_D))
        m_camera.processKeyboard(CCamera::RIGHT, 1.0f);
}

glm::mat4 CGLViewer::projectionMatrix()
{
    return glm::perspective(45.0f, (float)width()/(float)height(), 0.1f, 100000000000000.0f);;
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
    m_camera.setPosition(glm::vec3(0, 5, 0));
}

void CGLViewer::setAxisIsDrawn(bool drawn)
{
    QSettings().setValue("axisDrawn", drawn);
}

void CGLViewer::setGridIsDrawn(bool drawn)
{
    QSettings().setValue("gridDrawn", drawn);
}
