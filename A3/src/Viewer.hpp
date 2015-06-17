#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

#include "scene.hpp"
#include <stack>
#include <QMatrix4x4>

#define  SENS_PANX     30.0
#define  SENS_PANY     23.0
#define  SENS_ZOOM     35.0

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
    
    bool mPosOriMode;

    void resetPos();
    void resetOri();
    void resetJoints();

    void undo();
    void redo();

    void toggleTrackingCircle();
    void toggleZBuffer();
    void toggleBackface();
    void toggleFrontface();

    void drawSphere(bool picking = false);
    void set_colour(const QColor& col);

    std::stack<QMatrix4x4>* mMatStack;

protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when the window is resized (formerly on_configure_event)
    virtual void resizeGL(int width, int height);
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );
    
    // Draw a circle for the trackball, with OpenGL commands.
    // Assumes the context for the viewer is active.
    void draw_trackball_circle();

private:

    QMatrix4x4 getCameraMatrix();

    void recursiveDraw(SceneNode* node);
    void drawScene();

    void initializeScene();

    float* getSphereData();
    void addShaders();
    void initializeVertexBuffer();
    void initializeProgram();

    void updateCulling();

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
/*#else 
    QGLBuffer mCircleBufferObject;
#endif*/
    
    int mMvpMatrixLocation;
    int mCenterLocation;
    int mColorLocation;
    int mPickingLocation;
    int mLightLocation;

    int mSphereVertexCount;
    
    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mTranslateMatrix;
    QMatrix4x4 mRotateMatrix;
    QGLShaderProgram mProgram;

    SceneNode* mScene;
    
    int mButtonsPressed;
    int mPreviousX;
    int mPreviousY;

    bool mTrackingCircle;
    bool mZBuffer;
    bool mBackfaceCulling;
    bool mFrontfaceCulling;
};

#endif
