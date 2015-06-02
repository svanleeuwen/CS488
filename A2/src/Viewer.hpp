#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
// #include "algebra.hpp"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else 
#include <QGLBuffer>
#endif

#include <vector>
#include <QLabel>

class Viewer : public QGLWidget {
    
    Q_OBJECT

public:
    Viewer(const QGLFormat& format, QWidget *parent = 0);
    virtual ~Viewer();
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    enum Mode { rotateView, transView, perspective, rotateModel,
        transModel, scaleModel, viewport};

    // If you want to render a new frame, call do not call paintGL(),
    // instead, call update() to ensure that the view gets a paint 
    // event.

    // *** Fill in these functions (in viewer.cpp) ***

    void set_mode(Mode mode);

    // Set the parameters of the current perspective projection using
    // the semantics of gluPerspective().
    void set_perspective(double fov, double aspect,
                         double near, double far);

    void update_perspective();

    void set_viewport(QPoint& p1, QPoint& p2);

    // Restore all the transforms and perspective parameters to their
    // original state. Set the viewport to its initial size.
    void reset_view();

    QLabel* mNearText;
    QLabel* mFarText;

protected:

    // Events we implement

    // Called when GL is first initialized
    virtual void initializeGL();
    // Called when our window needs to be redrawn
    virtual void paintGL();
    // Called when a mouse button is pressed
    virtual void mousePressEvent ( QMouseEvent * event );
    // Called when a mouse button is released
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    // Called when the mouse moves
    virtual void mouseMoveEvent ( QMouseEvent * event );
 
    // Draw a line -- call draw_init first!
    void draw_line(const QVector2D& p1, const QVector2D& p2) ;

    // Set the current colour
    void set_colour(const QColor& col);

    // Call this before you begin drawing. Width and height are the width
    // and height of the GL window.
    void draw_init();
private:
    std::vector<QVector4D>* clip(QVector4D p1, QVector4D p2);
    enum Boundary { bl, br, bb, bt, bn, bf };

    void draw_cube(QMatrix4x4& modelToWorld, QMatrix4x4& worldToScreen);
    void draw_gnomon(const std::vector<QVector4D>& gnomon);
    void draw_model_gnomon(QMatrix4x4& worldToScreen);
    void draw_world_gnomon(QMatrix4x4& worldToScreen);
    void draw_viewport();

    void rotate_model(double x, double y, double z);
    void rotate_view(double x, double y, double z);

    void rot_model_x(QMatrix4x4& mat, double x);
    void rot_model_y(QMatrix4x4& mat, double y);
    void rot_model_z(QMatrix4x4& mat, double z);

    void rot_view_x(QMatrix4x4& mat, double x);
    void rot_view_y(QMatrix4x4& mat, double y);
    void rot_view_z(QMatrix4x4& mat, double z);
    
    void scale_model(double x, double y, double z);

    void translate_model(double x, double y, double z);
    void translate_view(double x, double y, double z);

    double clamp(double val, double low, double high);
    int clamp(int val, int low, int high);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    QOpenGLBuffer mVertexBufferObject;
    QOpenGLVertexArrayObject mVertexArrayObject;
#else 
    QGLBuffer mVertexBufferObject;
#endif

    QGLShaderProgram mProgram;

    int mColorLocation;

    Mode mMode;
    int mButtonsPressed;

    int mPreviousX;
    bool mMovingRight;
    
    std::vector<QVector4D> mCube;
    std::vector<std::vector<int>> mCubeIndices;
    std::vector<QVector4D> mModelGnomon;
    std::vector<QVector4D> mWorldGnomon;

    QMatrix4x4 m_modelScale;
    QMatrix4x4 m_modelRotTrans;

    QMatrix4x4 m_view;
    QMatrix4x4 m_perspective;

    QMatrix4x4 m_screen;

    double mNear;
    double mFar;
    double mFov;
    double mAspect;
    
    QVector2D mViewportBL;
    QVector2D mViewportTR;

    QPoint mPoint1;
    QPoint mPoint2;
};

#endif
