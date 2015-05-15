// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mElementBufferObject(QOpenGLBuffer::IndexBuffer)
    , mVertexArrayObject(this)
/*#else
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif */
{
    mGame = new Game();
    mGameTimer = new QTimer(this);
    connect(mGameTimer, SIGNAL(timeout()), this, SLOT(update()));
    mGameTimer->start(1000/30);

    mRotateTimer = new QTimer(this);
    mPersistenceTimer = new QTimer(this);

    mModelMatrices[0].translate(-6,-11);
    mModelMatrices[0].scale(1, 21);
    mModelMatrices[1].translate(5, -11);
    mModelMatrices[1].scale(1, 21);
    mModelMatrices[2].translate(-5, -11);
    mModelMatrices[2].scale(10, 1);
}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

Game* Viewer::getGame() {
    return mGame;
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 1.0, 0.0);

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

    GLfloat cubeData[] = {
        // X     Y     Z
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
    };

    GLushort indices[] = { 
      0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7,
      4, 6, 2, 0, 6, 7, 0, 1, 1, 7, 3, 5
    };

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mElementBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (!mElementBufferObject.bind()) {
        std::cerr << "could not bind Element buffer to the context." << std::endl;
        return;
    }
    mElementBufferObject.allocate(indices, sizeof(GLushort) * 4 * 6);

    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
/*#else

     *
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
     * QOpenGLBuffer.
     *
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
    mVertexBufferObject.setUsagePattern(QGLBuffer::StaticDraw);
#endif
*/
    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mVertexBufferObject.allocate(cubeData, 4 * 6 * 3 * sizeof(GLfloat));

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    // mPerspMatrixLocation = mProgram.uniformLocation("cameraMatrix");
    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mElementBufferObject.bind();
    mVertexArrayObject.bind();
//#endif

    drawWell();
}

void Viewer::drawWell()
{
    for(int j = 0; j < 3; j++) {
        for(int i = 0; i < 6; i++) {
            mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mModelMatrices[j]);
            glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 
                (const GLvoid*)(i*4*sizeof(GLushort)));
        }
    }
}
/*
void Viewer::drawPieces()
{
    for(int i = 0; i < 
}
*/
void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    mButtonPressed = event->button();
    mShiftPressed = event->modifiers() == Qt::ShiftModifier;
    mPreviousX = event->x();

    if(!mShiftPressed);
        // Start timer
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    mButtonPressed = Qt::NoButton;

    if(!mShiftPressed);
        // Stop timer

    // Start persistence

    // Call parent
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) { 
    if(mButtonPressed != Qt::LeftButton &&
            mButtonPressed != Qt::RightButton &&
            mButtonPressed != Qt::MiddleButton) {
        QGLWidget::mouseMoveEvent(event);
        return;
    }

    int deltaX = event->x() - mPreviousX;
    mPreviousX = event->x();
    // Update rotate speed variable
    
    if(deltaX != 0) {
        if(mMovingRight != (deltaX > 0)) {
            mMovingRight = deltaX > 0;
            
            if(!mShiftPressed);
                // Reset timer
        }
        
        if(!mShiftPressed) {
            float rotateFactor = deltaX / 2.5;

            switch(mButtonPressed) {
                case Qt::LeftButton:
                    rotateWorld(rotateFactor, 1, 0, 0);
                    break;

                case Qt::RightButton:
                    rotateWorld(rotateFactor, 0, 1, 0); 
                    break;

                case Qt::MiddleButton:
                    rotateWorld(rotateFactor, 0, 0, 1);
            }
        } else {
            float scaleFactor = 1 + deltaX/100.0f;

            if(mTotalScaling * scaleFactor > scaleMax) {
                scaleFactor = scaleMax/mTotalScaling;
            } else if(mTotalScaling * scaleFactor < scaleMin) {
                scaleFactor = scaleMin/mTotalScaling;
            }

            scaleWorld(scaleFactor, scaleFactor, scaleFactor);
            mTotalScaling = mTotalScaling * scaleFactor;
        }
    }
}

QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
    mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    mTransformMatrix.scale(x, y, z);
}

