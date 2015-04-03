#ifndef GLVIEWER_HPP
#define GLVIEWER_HPP
#include "core/CCamera.hpp"
#include "core/CKeyboardManager.hpp"
#include "core/SBoundingBox.hpp"

#include <Athena/Global.hpp>
#include <glm/glm.hpp>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include <memory>


class QListWidgetItem;
class IResource;
class IRenderableModel;

class CGLViewer : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit CGLViewer(QWidget* parent = 0);
    virtual ~CGLViewer();
    void exportFile();

    static CGLViewer* instance();

    glm::mat4 projectionMatrix();
    glm::mat4 modelMatrix();
    glm::mat4 viewMatrix();

    void setCurrent(IRenderableModel* renderable);
    void setSkybox(IRenderableModel* renderable);

    float frameRate() const;

    IRenderableModel* currentModel();
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
    QTime                            m_frameTimer;
    SBoundingBox                     m_sceneBounds;
    IRenderableModel*                m_currentRenderable;
    IRenderableModel*                m_skybox;
    static CGLViewer*                m_instance;
    CCamera                          m_camera;
    QTimer                           m_updateTimer;
    bool                             m_mouseEnabled;
    bool                             m_isInitialized;
    float                            m_lastTime;
    float                            m_currentTime;
    float                            m_deltaTime;
};

#endif // GLVIEWER_HPP
