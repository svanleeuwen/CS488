#include <QtWidgets>
#include <QtOpenGL>
#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Viewer.hpp"
// #include "draw.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

using namespace std;

#define PI 3.14159265
const double DEFAULT_FOV = 30 * PI / 180.0;

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
#else 
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
{
    mNearText = new QLabel(this);
    mFarText = new QLabel(this);

    mCube = vector<QVector4D>({
        QVector4D(-1.0f, -1.0f, -1.0f, 1.0f),
        QVector4D(-1.0f, -1.0f, 1.0f, 1.0f),
        QVector4D(-1.0f, 1.0f, -1.0f, 1.0f),
        QVector4D(-1.0f, 1.0f, 1.0f, 1.0f),
        QVector4D(1.0f, -1.0f, -1.0f, 1.0f),
        QVector4D(1.0f, -1.0f, 1.0f, 1.0f),
        QVector4D(1.0f, 1.0f, -1.0f, 1.0f),
        QVector4D(1.0f, 1.0f, 1.0f, 1.0f)
    });

    mCubeIndices = {
            {0, 1}, {1, 3}, {3, 2}, {2, 0},
            {4, 5}, {5, 7}, {7, 6}, {6, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
       
    mModelGnomon = vector<QVector4D>({
        QVector4D(0.0f, 0.0f, 0.0f, 1.0f),
        QVector4D(0.5f, 0.0f, 0.0f, 1.0f),
        QVector4D(0.0f, 0.5f, 0.0f, 1.0f),
        QVector4D(0.0f, 0.0f, 0.5f, 1.0f)
    });

    mWorldGnomon = vector<QVector4D>({
        QVector4D(0.0f, 0.0f, 0.0f, 1.0f),
        QVector4D(0.5f, 0.0f, 0.0f, 1.0f),
        QVector4D(0.0f, 0.5f, 0.0f, 1.0f),
        QVector4D(0.0f, 0.0f, 0.5f, 1.0f)
    });
}

Viewer::~Viewer() {
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 300);
}

void Viewer::set_mode(Mode mode) {
    mMode = mode;
}

void Viewer::set_perspective(double fov, double aspect,
                             double near, double far)
{
    m_perspective.setToIdentity();
    
    double y = 1/tan(fov/2);
    m_perspective(0,0) = y / aspect;
    m_perspective(1,1) = y;

    m_perspective(2,2) = (far+near)/(far-near);
    m_perspective(2,3) = (-2*far*near)/(far-near);
    m_perspective(3,2) = 1;
    m_perspective(3,3) = 0;

    mNearText->setText(QString(("Near: " + to_string(near)).c_str()));
    mFarText->setText(QString(("Far: " + to_string(far)).c_str()));
}

void Viewer::update_perspective() {
    mAspect = tan(DEFAULT_FOV/2.0)/tan(mFov/2.0);

    float lx = mViewportBL.x();
    float ly = mViewportBL.y();
    float hx = mViewportTR.x();
    float hy = mViewportTR.y();

    m_screen.setToIdentity();
    
    m_screen(0,0) = (hx-lx)/2.0;
    m_screen(1,1) = (hy-ly)/2.0;
    m_screen(0,3) = (lx+hx)/2.0;
    m_screen(1,3) = (ly+hy)/2.0;

    set_perspective(mFov, mAspect, mNear, mFar);
}

void Viewer::set_viewport(QPoint& p1, QPoint& p2) {
    mViewportBL = QVector2D(min(p1.x(), p2.x())*2.0/width() - 1, -(min(p1.y(), p2.y())*2.0/height() - 1));
    mViewportTR = QVector2D(max(p1.x(), p2.x())*2.0/width() - 1, -(max(p1.y(), p2.y())*2.0/height() - 1));

    update_perspective();
}

void Viewer::reset_view() {
    m_modelScale.setToIdentity();
    m_modelRotTrans.setToIdentity();

    m_view.setToIdentity();
    rotate_view(PI, 0.0, 0.0);
    translate_view(0, 0, 10);

    mNear = 1;
    mFar = 25;
    mFov = DEFAULT_FOV;

    int wMargin = width()/20;
    int hMargin = height()/20;

    QPoint bottomleft = QPoint(0 + wMargin, 0 + hMargin);
    QPoint topright = QPoint(width() - wMargin, height() - hMargin);

    set_viewport(bottomleft, topright);
    update();

    set_mode(Mode::rotateModel);
}

void Viewer::initializeGL() {
    // Do some OpenGL setup
    QGLFormat glFormat = QGLWidget::format();

    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 0.7, 0.0);
    
    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
        std::cerr << "Cannot load vertex shader." << std::endl;
        return;
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
        std::cerr << "Cannot load fragment shader." << std::endl;
        return;
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::DynamicDraw);
#else 

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of 
     * QOpenGLBuffer.
     */
    uint vao;
     
    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);
     
    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;
     
    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");
     
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);    

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QGLBuffer::DynamicDraw);
#endif

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mColorLocation = mProgram.uniformLocation("frag_color");

    reset_view();
}



vector<QVector4D>* Viewer::clip(QVector4D p1, QVector4D p2) {
    float c1[6] = {0};
    float c2[6] = {0};

    c1[Boundary::bl] = p1.w() + p1.x();
    c1[Boundary::br] = p1.w() - p1.x();
    c1[Boundary::bb] = p1.w() + p1.y();
    c1[Boundary::bt] = p1.w() - p1.y();
    c1[Boundary::bn] = p1.w() + p1.z();
    c1[Boundary::bf] = p1.w() - p1.z();

    c2[Boundary::bl] = p2.w() + p2.x();
    c2[Boundary::br] = p2.w() - p2.x();
    c2[Boundary::bb] = p2.w() + p2.y();
    c2[Boundary::bt] = p2.w() - p2.y();
    c2[Boundary::bn] = p2.w() + p2.z();
    c2[Boundary::bf] = p2.w() - p2.z();

    bool accept = true;

    for(int i = 0; i < 6; i++) {
        if(c1[i] < 0 || c2[i] < 0) {
            accept = false;
        }
        if(c1[i] < 0 && c2[i] < 0) {
            return NULL;
        }
    }
    
    if(accept) {
        return new vector<QVector4D>({p1, p2});
    }

    for(int i = 0; i < 6; i++) {
        if(c1[i] < 0 || c2[i] < 0) {
            double t = c1[i]/(c1[i] - c2[i]);
            QVector4D intersection = (1-t)*p1 + t*p2;

            if(c1[i] < 0) {
                p1 = intersection;
                
                c1[Boundary::bl] = p1.w() + p1.x();
                c1[Boundary::br] = p1.w() - p1.x();
                c1[Boundary::bb] = p1.w() + p1.y();
                c1[Boundary::bt] = p1.w() - p1.y();
                c1[Boundary::bn] = p1.w() + p1.z();
                c1[Boundary::bf] = p1.w() - p1.z();
            }

            if(c2[i] < 0) {
                p2 = intersection;

                c2[Boundary::bl] = p2.w() + p2.x();
                c2[Boundary::br] = p2.w() - p2.x();
                c2[Boundary::bb] = p2.w() + p2.y();
                c2[Boundary::bt] = p2.w() - p2.y();
                c2[Boundary::bn] = p2.w() + p2.z();
                c2[Boundary::bf] = p2.w() - p2.z();
            }
        }
    }

    return new vector<QVector4D>({p1, p2});
}

void Viewer::draw_cube(QMatrix4x4& modelToWorld, QMatrix4x4& worldToNDC) {
    vector<QVector4D> cube = mCube;

    for(uint i = 0; i < cube.size(); i++) {
        cube[i] = worldToNDC * modelToWorld * cube[i];
    }

    for(uint a = 0; a < mCubeIndices.size(); a++) {
        int i = mCubeIndices[a][0];
        int j = mCubeIndices[a][1];

        vector<QVector4D>* clippedPoints = clip(cube[i], cube[j]);
       
        if(clippedPoints != NULL) {
            clippedPoints->at(0) = m_screen * clippedPoints->at(0);
            clippedPoints->at(1) = m_screen * clippedPoints->at(1);
 
            QVector2D p1 = QVector2D(clippedPoints->at(0))/clippedPoints->at(0).w();
            QVector2D p2 = QVector2D(clippedPoints->at(1))/clippedPoints->at(1).w();
            draw_line(p1, p2);
            
            delete clippedPoints;
        }
    }
}

void Viewer::draw_gnomon(const vector<QVector4D>& gnomon) {
    for(uint i = 1; i < gnomon.size(); i++) {
        vector<QVector4D>* clippedPoints = clip(gnomon[0], gnomon[i]);

        if(clippedPoints != NULL) {
            clippedPoints->at(0) = m_screen * clippedPoints->at(0);
            clippedPoints->at(1) = m_screen * clippedPoints->at(1);

            QVector2D p1 = QVector2D(clippedPoints->at(0))/clippedPoints->at(0).w();
            QVector2D p2 = QVector2D(clippedPoints->at(1))/clippedPoints->at(1).w();
            draw_line(p1, p2);

            delete clippedPoints;
        }
    }
}

void Viewer::draw_model_gnomon(QMatrix4x4& worldToNDC) {
    vector<QVector4D> gnomon = mModelGnomon;

    for(uint i = 0; i < gnomon.size(); i++) {
        gnomon[i] = worldToNDC * m_modelRotTrans * gnomon[i];
    }

    draw_gnomon(gnomon); 
}

void Viewer::draw_world_gnomon(QMatrix4x4& worldToNDC) {
    vector<QVector4D> gnomon = mWorldGnomon;

    for(uint i = 0; i < gnomon.size(); i++) {
        gnomon[i] = worldToNDC * gnomon[i];
    }

    draw_gnomon(gnomon);
}

void Viewer::draw_viewport() {
    draw_line(mViewportBL, QVector2D(mViewportBL.x(), mViewportTR.y()));
    draw_line(mViewportBL, QVector2D(mViewportTR.x(), mViewportBL.y()));
    draw_line(QVector2D(mViewportBL.x(), mViewportTR.y()), mViewportTR);
    draw_line(QVector2D(mViewportTR.x(), mViewportBL.y()), mViewportTR);
}

void Viewer::paintGL() {    
    draw_init();

    QMatrix4x4 modelToWorld = m_modelRotTrans * m_modelScale;
    QMatrix4x4 worldToNDC = m_perspective * m_view;

    set_colour(QColor(1.0, 1.0, 1.0));

    draw_cube(modelToWorld, worldToNDC);
    draw_model_gnomon(worldToNDC);
    draw_world_gnomon(worldToNDC);
    draw_viewport();
}

double Viewer::clamp(double val, double low, double high) {
    return max(low, min(high, val));
}

int Viewer::clamp(int val, int low, int high) {
    return max(low, min(high, val));
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    mButtonsPressed += event->button();
    mPreviousX = event->x();

    if(mMode == Mode::viewport && event->button() == Qt::LeftButton) {
        mPoint1 = QPoint(clamp(event->x(), 0, width()), clamp(event->y(), 0, height()));
    }
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    mButtonsPressed -= event->button();
}

const float F_TRANSLATE = 5.0f;
const float F_SCALE = 1.5f;
const float F_FOV = 1.5f;
const float F_NEAR = 5.0f;
const float F_FAR = 10.0f;

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    int deltaX = clamp(event->x(), 0, width()) - mPreviousX;
    mPreviousX = clamp(event->x(), 0, width());

    if(deltaX != 0) {
        double ratio = ((double)deltaX) / width();
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;

        if((mButtonsPressed & Qt::LeftButton) > 0) {
            x = ratio;
        }

        if((mButtonsPressed & Qt::MiddleButton) > 0) {
            y = ratio; 
        }

        if((mButtonsPressed & Qt::RightButton) > 0) {
            z = ratio; 
        }

        switch(mMode) {
            case rotateView:
                rotate_view(PI*x, PI*y, PI*z); 
                break;
            case transView:
                translate_view(F_TRANSLATE*x,F_TRANSLATE*y,F_TRANSLATE*z);
                break;
            case perspective:
                mFov = clamp(mFov + x*F_FOV, 5.0*PI/180.0, 160.0*PI/180);
                mNear = clamp(mNear + y*F_NEAR, 0.001, 10000.0);
                mFar = clamp(mFar + z*F_FAR, mNear, 10000.0);
                update_perspective();
                break;

            case rotateModel:
                rotate_model(2*PI*x, 2*PI*y, 2*PI*z);
                break;
            case transModel:
                translate_model(F_TRANSLATE*x,F_TRANSLATE*y,F_TRANSLATE*z);
                break;
            case scaleModel:
                scale_model(x*F_SCALE+1, y*F_SCALE + 1, z*F_SCALE+1);
                break;

            case viewport:
                if(x != 0.0) {
                    mPoint2 = QPoint(clamp(event->x(), 0, width()), clamp(event->y(), 0, height()));
                    set_viewport(mPoint1, mPoint2);
                }
        }
    }

    update();
}

void Viewer::rotate_model(double x, double y, double z) {
    rot_model_x(m_modelRotTrans, x);
    rot_model_y(m_modelRotTrans, y);
    rot_model_z(m_modelRotTrans, z);
}

void Viewer::rotate_view(double x, double y, double z) {
    rot_view_x(m_view, x);
    rot_view_y(m_view, y);
    rot_view_z(m_view, z);    
}

void Viewer::rot_model_x(QMatrix4x4 &mat, double x) {
    QMatrix4x4 rotMat;
    rotMat(1, 1) = cos(x);
    rotMat(1, 2) = -sin(x);
    rotMat(2, 1) = sin(x);
    rotMat(2, 2) = cos(x);

    mat = mat * rotMat;
}

void Viewer::rot_model_y(QMatrix4x4 &mat, double y) {
    QMatrix4x4 rotMat;
    rotMat(0, 0) = cos(y);
    rotMat(2, 0) = -sin(y);
    rotMat(0, 2) = sin(y);
    rotMat(2, 2) = cos(y);

    mat = mat * rotMat;
}

void Viewer::rot_model_z(QMatrix4x4 &mat, double z) {
    QMatrix4x4 rotMat;
    rotMat(0, 0) = cos(z);
    rotMat(0, 1) = -sin(z);
    rotMat(1, 0) = sin(z);
    rotMat(1, 1) = cos(z);

    mat = mat * rotMat;
}

void Viewer::rot_view_x(QMatrix4x4 &mat, double x) {
    QMatrix4x4 rotMat;
    rotMat(1, 1) = cos(x);
    rotMat(2, 1) = -sin(x);
    rotMat(1, 2) = sin(x);
    rotMat(2, 2) = cos(x);

    mat = rotMat * mat;
}

void Viewer::rot_view_y(QMatrix4x4 &mat, double y) {
    QMatrix4x4 rotMat;
    rotMat(0, 0) = cos(y);
    rotMat(0, 2) = -sin(y);
    rotMat(2, 0) = sin(y);
    rotMat(2, 2) = cos(y);

    mat = rotMat * mat;
}

void Viewer::rot_view_z(QMatrix4x4 &mat, double z) {
    QMatrix4x4 rotMat;
    rotMat(0, 0) = cos(z);
    rotMat(1, 0) = -sin(z);
    rotMat(0, 1) = sin(z);
    rotMat(1, 1) = cos(z);

    mat = rotMat * mat;
}

void Viewer::scale_model(double x, double y, double z) {
    QMatrix4x4 scaleMat;
    scaleMat(0, 0) = x;
    scaleMat(1, 1) = y;
    scaleMat(2, 2) = z;

    m_modelScale = scaleMat * m_modelScale;
}

void Viewer::translate_model(double x, double y, double z) {
    QMatrix4x4 transMat;
    transMat(0, 3) = x;
    transMat(1, 3) = y;
    transMat(2, 3) = z;

    m_modelRotTrans = m_modelRotTrans * transMat;
}

void Viewer::translate_view(double x, double y, double z) {
    QMatrix4x4 transMat;
    transMat(0, 3) = x;
    transMat(1, 3) = y;
    transMat(2, 3) = z;

    m_view = transMat * m_view;
}

// Drawing Functions
// ******************* Do Not Touch ************************ // 

void Viewer::draw_line(const QVector2D& p1, const QVector2D& p2) {

  const GLfloat lineVertices[] = {
    p1.x(), p1.y(), 1.0,
    p2.x(), p2.y(), 1.0
  };

  mVertexBufferObject.allocate(lineVertices, 3 * 2 * sizeof(float));

  glDrawArrays(GL_LINES, 0, 2);
}

void Viewer::set_colour(const QColor& col)
{
  mProgram.setUniformValue(mColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::draw_init() {
    glClearColor(0.7, 0.7, 0.7, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)width(), 0.0, (double)height());
    glViewport(0, 0, width(), height());

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
}
