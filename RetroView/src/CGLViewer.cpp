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
#include "CGLViewer.hpp"
#include "CModelLoader.hpp"
#include "CModelLoaderThead.hpp"
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
      m_isInitialized(false),
      m_defaultVertexShader(nullptr),
      m_defaultFragmentShader(nullptr),
      m_defaultShaderProgram(nullptr)
{
    grabKeyboard();

    CResourceManager* resourceManager = CResourceManager::instance().get();
    resourceManager->initialize("/media/Storage/WiiImages/games/Metroid Prime 1.iso_dir");

    QGLWidget::setMouseTracking(true);
    m_instance = this;
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start();

    // Set our scene bounds
    m_sceneBounds.min = glm::vec3(-100.f);
    m_sceneBounds.max = glm::vec3( 100.f);

    for (int i = Qt::Key_A; i < Qt::Key_Z; i++)
        m_keys[(Qt::Key)i] = false;
}

CGLViewer::~CGLViewer()
{
    foreach (IRenderableModel* file, m_renderables)
    {
        delete file;
        file = NULL;
    }

    m_renderables.clear();


    std::cout << "I'M DYING!!!" << std::endl;

    releaseKeyboard();
}

void CGLViewer::paintGL()
{
    QGLWidget::paintGL();
    if (!m_defaultShaderProgram)
        return;

    //gLight.position = glm::vec4(m_camera.position(), 0.0);
    //lights[0].coneDirection = m_camera.front();
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    //lights[2].coneDirection = m_camera.front();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 modelMat = modelMatrix();
    glm::mat4 viewMat = viewMatrix();
    glm::mat4 projectionMat = projectionMatrix();
    glm::mat4 mvp = projectionMat * viewMat * modelMat;
    glLoadMatrixf(&mvp[0][0]);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_COLOR_MATERIAL);
    glPrimitiveRestartIndex(0xFFFFFFFF);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    //    glEnable(GL_CULL_FACE);
    if (QSettings().value("wireframe").toBool())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


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
        m_defaultShaderProgram->bind();
        m_defaultShaderProgram->setUniformValue("backfaceCulling", (int)true);
        atUint32 modelLoc = m_defaultShaderProgram->uniformLocation("model");
        CAreaFile* test = dynamic_cast<CAreaFile*>(m_currentRenderable);
        if (test)
        {
            glm::mat4 tmpMat = glm::mat4(test->transformMatrix());
            modelMat = glm::inverse(glm::transpose(tmpMat));
        }

        atUint32 viewLoc = m_defaultShaderProgram->uniformLocation("view");
        atUint32 projectionLoc = m_defaultShaderProgram->uniformLocation("projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMat[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMat[0][0]);

        GLint camPosLoc = glGetUniformLocation(m_defaultShaderProgram->programId(), "cameraPosition");
        glUniform3fv(camPosLoc, 1, &m_camera.position()[0]);
        m_currentRenderable->draw();

        m_defaultShaderProgram->setUniformValue("backfaceCulling", (int)false);
        if (QSettings().value("drawBoundingBox").toBool())
            drawBoundingBox(m_currentRenderable->boundingBox());
        m_defaultShaderProgram->release();
    }

    drawBoundingBox(m_sceneBounds);
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

        if ((GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
        {
            m_defaultVertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
            m_defaultVertexShader->compileSourceFile(":/shaders/default_vertex.glsl");
            m_defaultFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
            m_defaultFragmentShader->compileSourceFile(":/shaders/default_fragment.glsl");
            m_defaultShaderProgram = new QOpenGLShaderProgram(this);
            m_defaultShaderProgram->addShader(m_defaultVertexShader);
            m_defaultShaderProgram->addShader(m_defaultFragmentShader);
            m_defaultShaderProgram->link();
        }

        qglClearColor(QColor(128, 128, 128).darker());
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

void CGLViewer::focusOutEvent(QFocusEvent* e)
{
    for (int i = Qt::Key_A; i < Qt::Key_Z; i++)
        m_keys[(Qt::Key)i] = false;
    QGLWidget::focusOutEvent(e);
}

void CGLViewer::keyPressEvent(QKeyEvent* e)
{
    m_keys[(Qt::Key)e->key()] = true;

    QGLWidget::keyPressEvent(e);
}

void CGLViewer::keyReleaseEvent(QKeyEvent* e)
{
    m_keys[(Qt::Key)e->key()] = false;

    QGLWidget::keyReleaseEvent(e);
}

bool CGLViewer::hasFile(const QString& file)
{
    return m_renderables.keys().contains(file);
}

QOpenGLShaderProgram* CGLViewer::defaultShader()
{
    return m_defaultShaderProgram;
}

void CGLViewer::updateCamera()
{
    if (m_keys[Qt::Key_W])
        m_camera.processKeyboard(CCamera::FORWARD, 1.0f);
    if (m_keys[Qt::Key_S])
        m_camera.processKeyboard(CCamera::BACKWARD, 1.0f);
    if (m_keys[Qt::Key_A])
        m_camera.processKeyboard(CCamera::LEFT, 1.0f);
    if (m_keys[Qt::Key_D])
        m_camera.processKeyboard(CCamera::RIGHT, 1.0f);
}

void CGLViewer::openModels(const QStringList& files)
{
    qApp->setOverrideCursor(Qt::BusyCursor);
    QThread* thread = new QThread;
    ModelLoaderThread* worker = new ModelLoaderThread(files);
    worker->moveToThread(thread);
    connect(worker, SIGNAL(error(QString)), this, SLOT(onError(QString)));
    connect(worker, SIGNAL(newFile(IRenderableModel*,QString)), this, SLOT(onNewFile(IRenderableModel*,QString)));
    connect(worker, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

IRenderableModel* CGLViewer::renderable(const atUint64 assetId)
{
    if (assetId == 0 || (atInt64)assetId == -1)
        return nullptr;

    foreach (IRenderableModel* model, m_renderables)
    {
        if (model->assetId() == assetId)
            return model;
    }

    return nullptr;
}

void CGLViewer::addRenderable(const QString& path, IRenderableModel* renderable)
{
    QString tmpFilename = path;
#ifdef Q_OS_WIN32
    tmpFilename = path.toLower();
#endif
    if (m_renderables.contains(tmpFilename))
        return;

    m_renderables[tmpFilename] = renderable;
}

void CGLViewer::removeRenderable(IRenderableModel* renderable)
{
    m_renderables.remove(m_renderables.key(renderable));
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

void CGLViewer::stopUpdates()
{
    m_updateTimer.stop();
}

void CGLViewer::startUpdates()
{
    m_updateTimer.start();
}

void CGLViewer::exportFile(const QString& file)
{
    QString tmpFilename = file;
#ifdef Q_OS_WIN32
    tmpFilename = file.toLower();
#endif
    if (m_renderables.contains(tmpFilename) && m_renderables[tmpFilename]->canExport())
    {
        QString exportPath = QFileDialog::getSaveFileName(this, "Export Model...", QString(), "Wavefron Obj *.obj (*.obj)");
        if (!exportPath.isEmpty())
        {
            if (!exportPath.contains(".obj"))
                exportPath += ".obj";

            m_renderables[tmpFilename]->exportToObj(exportPath.toStdString());
        }
    }
}

CGLViewer* CGLViewer::instance()
{
    return m_instance;
}

void CGLViewer::closeFile(const QString& filename)
{
    QString tmpFilename = filename;
#ifdef Q_OS_WIN32
    tmpFilename = filename.toLower();
#endif
    if (m_renderables.contains(tmpFilename))
    {
        IRenderableModel* file = m_renderables[tmpFilename];
        m_renderables.remove(tmpFilename);

        delete file;
        file = NULL;
        m_currentRenderable = nullptr;
        update();
        return;
    }
}

void CGLViewer::onItemSelectionChanged()
{
    QListWidget* listWidget = qobject_cast<QListWidget*>(sender());
    if (listWidget && listWidget->currentItem())
    {
        QString path = listWidget->currentItem()->data(Qt::UserRole).toString();
#ifdef Q_OS_WIN32
        path = path.toLower();
#endif
        if (m_renderables.contains(path))
            m_currentRenderable = m_renderables[path];
    }
}

void CGLViewer::resetCamera()
{
    /*
    m_camera.reset();
    m_camera.setLookAt(glm::vec3(0, -10, 0));
    m_camera.setPosition(glm::vec3(0, 5, 0));*/
}

void CGLViewer::setAxisIsDrawn(bool drawn)
{
    QSettings().setValue("axisDrawn", drawn);
}

void CGLViewer::setGridIsDrawn(bool drawn)
{
    QSettings().setValue("gridDrawn", drawn);
}

void CGLViewer::onError(QString message)
{
    QMessageBox::warning(this, "Error loading model", message);
}

void CGLViewer::onNewFile(IRenderableModel* renderable, QString path)
{
    QMutexLocker lock(&globalMutex);
    QString tmpFilename = path;
#ifdef Q_OS_WIN32
    tmpFilename = tmpFilename.toLower();
#endif
    m_renderables[tmpFilename] = renderable;
    emit fileAdded(path);
    lock.unlock();
}

void CGLViewer::onFinished()
{
    qApp->restoreOverrideCursor();
}
