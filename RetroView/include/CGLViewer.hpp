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
    void openModels(QStringList files);
    void openSkeleton(const QString& file);
    void openMap(const QString& file);
    void exportFile(const QString& file);

    static CGLViewer* instance();

    void addRenderable(const QString& path, IRenderableModel* renderable);
    void removeRenderable(IRenderableModel* renderable);

    glm::mat4 projectionMatrix();
    glm::mat4 modelMatrix();
    glm::mat4 viewMatrix();

    IRenderableModel* renderable(const atUint64 assetId);
    void setCurrent(IRenderableModel* renderable);

    // Returns true if the path alreadys exists
    // Assumes the filepath is already properly formatted (lowercase on windows)
    bool hasFile(const QString& file);

public slots:
    void stopUpdates();
    void startUpdates();
    void closeFile(const QString& filename);
    void onItemSelectionChanged();
    void resetCamera();
    void setAxisIsDrawn(bool drawn);
    void setGridIsDrawn(bool drawn);

signals:
    void initialized();
    void closing();
    void fileAdded(QString);

private slots:
    void onError(QString message);
    void onNewFile(IResource* renderable, QString path);
    void onFinished();
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
    QMap<QString, IRenderableModel*> m_renderables;
    IRenderableModel*                m_currentRenderable;
    static CGLViewer*                m_instance;
    CCamera                          m_camera;
    QTimer                           m_updateTimer;
    bool                             m_mouseEnabled;
    bool                             m_isInitialized;
};

#endif // GLVIEWER_HPP
