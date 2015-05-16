#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include "game.hpp"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    Game* getGame();

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.
  
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

private slots:
    virtual void persistenceRotate(); 
    virtual void tick();

private:
    virtual void drawCube(int colourIndex);
    virtual void drawWell();
    virtual void drawPieces();
  

    QMatrix4x4 getCameraMatrix();
    void translateWorld(float x, float y, float z);
    void rotateWorld(float angle, float x, float y, float z);
    void scaleWorld(float x, float y, float z);


//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLBuffer mElementBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
/*#else 
    QGLBuffer mVertexBufferObject;
#endif */

    GLint mVertexLocation;
    GLint mMvpMatrixLocation;
    GLint mColourLocation;

    QMatrix4x4 mPerspMatrix;
    QMatrix4x4 mModelMatrices[3];
    QMatrix4x4 mTransformMatrix;

    Game* mGame;    
    QTimer* mGameTimer;

    QTimer* mRotateTimer;
    QTimer* mPersistenceTimer;

    int mButtonPressed;
    bool mShiftPressed;

    int mPreviousX;
    bool mMovingRight;
    float mPersistenceSpeed;
    QVector3D mPersistenceAxis;
    
    int mTotalX = 0;
    float mTotalScaling = 1.0f;

    const float mScaleMax = 1.5f;
    const float mScaleMin = 0.25f;

    QGLShaderProgram mProgram;
};

#endif
