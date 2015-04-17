#ifndef GLVIEWER_HPP
#define GLVIEWER_HPP
#include "core/CCamera.hpp"
#include "core/CKeyboardManager.hpp"
#include "core/SBoundingBox.hpp"

#include <Athena/Global.hpp>
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

    CProjection projection();
    CTransform model();
    CTransform view();
    CVector3f cameraPosition();
    CVector3f cameraVector();
    CQuaternion cameraRotation();

    void setCurrent(IRenderableModel* renderable);
    void setSkybox(IRenderableModel* renderable);

    float frameRate() const;

    IRenderableModel* currentModel();
    void drawSky();

    void setSkyVisible(bool visible);
    float movementSpeed() const;
    void setMovementSpeed(float speed);
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
    void movementSpeedChanged(float);

protected :
    void paintGL();
    void resizeGL(int w, int h);
    atUint32 texture(atUint64 texId, atUint32 size, atUint32 version, QString searchDir);
    virtual void initializeGL();

protected:
    void closeEvent(QCloseEvent*);
    void wheelEvent(QWheelEvent* e);
private:
    void updateCamera(float delta);
    QTime                            m_frameTimer;
    IRenderableModel*                m_currentRenderable;
    IRenderableModel*                m_skybox;
    static CGLViewer*                m_instance;
    CCamera                          m_camera;
    QTimer                           m_updateTimer;
    Qt::MouseButtons                 m_buttons;
    bool                             m_isInitialized;
    bool                             m_skyVisible;
    float                            m_lastTime;
    float                            m_currentTime;
    float                            m_deltaTime;
};

#endif // GLVIEWER_HPP
