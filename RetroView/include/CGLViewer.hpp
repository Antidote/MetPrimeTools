#ifndef GLVIEWER_HPP
#define GLVIEWER_HPP

#include <Athena/Global.hpp>
#include <glm/glm.hpp>
#include <QtOpenGL/QGLWidget>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <QTimer>
#include <memory>

#include "CCamera.hpp"
#include "CKeyboardManager.hpp"
#include "SBoundingBox.hpp"

class QListWidgetItem;
class IResource;
class IRenderableModel;

class CGLViewer : public QGLWidget
{
    Q_OBJECT
public:
    explicit CGLViewer(QWidget* parent = 0);
    ~CGLViewer();
    void exportFile();

    static CGLViewer* instance();

    glm::mat4 projectionMatrix();
    glm::mat4 modelMatrix();
    glm::mat4 viewMatrix();

    void setCurrent(IRenderableModel* renderable);

public slots:
    void stopUpdates();
    void startUpdates();
    void resetCamera();
    void setAxisIsDrawn(bool drawn);
    void setGridIsDrawn(bool drawn);

signals:
    void initialized();
    void closing();
    void fileAdded(QString);

protected :
    void paintGL();
    void resizeGL(int w, int h);
    atUint32 texture(atUint64 texId, atUint32 size, atUint32 version, QString searchDir);
    virtual void initializeGL();

protected:
    void closeEvent(QCloseEvent *);
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);
private:
    void updateCamera();
    float                            m_cameraSpeed;
    QTime                            m_frameTimer;
    SBoundingBox                     m_sceneBounds;
    glm::vec3                        m_position;
    glm::mat4                        m_projectionMatrix;
    glm::mat4                        m_viewMatrix;
    IRenderableModel*                m_currentRenderable;
    static CGLViewer*                m_instance;
    CCamera                          m_camera;
    QTimer                           m_updateTimer;
    bool                             m_mouseEnabled;
    bool                             m_isInitialized;
};

#endif // GLVIEWER_HPP
