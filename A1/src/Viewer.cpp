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

static const QColor COLOURS[] = {
    QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255),
    QColor(255, 255, 0), QColor(0, 255, 255), QColor(255, 0, 255),
    QColor(255, 165, 0), QColor(150, 125, 255), QColor(200, 200, 200)
};

Viewer::Viewer(const QGLFormat& format, Game* game, QWidget *parent)
    : QGLWidget(format, parent)
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mElementBufferObject(QOpenGLBuffer::IndexBuffer)
    , mVertexArrayObject(this)
    , mGame(game)
/*#else
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif */
{
    mUpdateTimer = new QTimer(this);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(update()));
    mUpdateTimer->start(1000/30);

    mRotateTimer = new QTimer(this);
    mRotateTimer->setSingleShot(true);
    mRotateTimer->setInterval(75);

    mReleaseTimer = new QTimer(this);
    mReleaseTimer->setSingleShot(true);
    mReleaseTimer->setInterval(25);

    mPersistenceTimer = new QTimer(this);
    mPersistenceTimer->setInterval(50);
    connect(mPersistenceTimer, SIGNAL(timeout()), this, SLOT(persistenceRotate()));

    mModelMatrices[0].translate(-6,-11);
    mModelMatrices[0].scale(1, 21);
    mModelMatrices[1].translate(5, -11);
    mModelMatrices[1].scale(1, 21);
    mModelMatrices[2].translate(-5, -11);
    mModelMatrices[2].scale(10, 1);
    
    mButtonsPressed = Qt::NoButton;
}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

void Viewer::setDrawMode(DrawMode mode) {
    mMode = mode;
    if(mMode == DrawMode::wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Viewer::resetView() {
    mTransformMatrix.setToIdentity();
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
        1.0f, 0.0f, 1.0f
    };

    GLushort indices[] = { 
      0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7,
      4, 6, 2, 0, 6, 7, 0, 1, 1, 7, 3, 5
    };

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mElementBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    if (!mElementBufferObject.bind()) {
        std::cerr << "could not bind Element buffer to the context." << std::endl;
        return;
    }
    mElementBufferObject.allocate(indices, sizeof(GLushort) * 4 * 6);

    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::DynamicDraw);
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
    mColourLocation = mProgram.uniformLocation("colour");

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mElementBufferObject.bind();
    mVertexArrayObject.bind();
//#endif

    drawWell();
    drawPieces();
}

void Viewer::drawCube(int colourIndex)
{
    QColor colour;
    
    if(mMode != DrawMode::multicolour)
    {
        colour = COLOURS[colourIndex];
        mProgram.setUniformValue(mColourLocation, colour);
    }
            
    for(int i = 0; i < 6; i++) {
        if(mMode == DrawMode::multicolour)
        {
            colour = COLOURS[colourIndex];
            colour.setRedF(colour.redF()*(i+1)/7);
            colour.setGreenF(colour.greenF()*(i+1)/7);
            colour.setBlueF(colour.blueF()*(i+1)/7);

            mProgram.setUniformValue(mColourLocation, colour);
        }

        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 
            (const GLvoid*)(i*4*sizeof(GLushort)));
    }
}

void Viewer::drawWell()
{
    for(int j = 0; j < 3; j++) {
        mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mModelMatrices[j]); 
        drawCube(8);
    }
}

void Viewer::drawPieces()
{
    for(int i = 0; i < mGame->getHeight() - 4; i++)
    {
        for(int j = 0; j < mGame->getWidth(); j++)
        {
            QMatrix4x4 modelMatrix;
            modelMatrix.translate(-5 + j, -10 + i);
            mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * modelMatrix);
           
            int colour = mGame->get(i, j); 
            if(colour > -1)
                drawCube(colour); 
        }
    }
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    if(mButtonsPressed == Qt::NoButton) {
        mShiftPressed = event->modifiers() == Qt::ShiftModifier;
    }

    mButtonsPressed += event->button();
    mPreviousX = event->x();

    if(mPersistenceTimer->isActive()) {
        mPersistenceTimer->stop();
    }
}


void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    static int buttonsReleased = Qt::NoButton;

    int remainingReleaseTime = mReleaseTimer->remainingTime();
    if(remainingReleaseTime <= 0) {
        buttonsReleased = Qt::NoButton;
    }

    buttonsReleased += event->button();
    mButtonsPressed -= event->button();

    if(!mShiftPressed && (mButtonsPressed == Qt::NoButton)) {
        int remainingTime = mRotateTimer->remainingTime();
        
        if(remainingTime > 0) {
            mPersistenceAxis = {0, 0, 0};

            if((buttonsReleased & Qt::LeftButton) > 0) {
                mPersistenceAxis += {1, 0, 0};
            }

            if((buttonsReleased & Qt::RightButton) > 0) {
                mPersistenceAxis += {0, 1, 0};
            }

            if((buttonsReleased & Qt::MiddleButton) > 0) {
                mPersistenceAxis += {0, 0, 1};
            }

            mPersistenceSpeed = mPersistenceTimer->interval()*mTotalX/remainingTime;
            mPersistenceTimer->start();
        }
        
    } else if(!mShiftPressed) {
        mReleaseTimer->start();
    }
}

const float SCALE_DIVISOR = 100.0f;
const float ROTATE_DIVISOR = 2.5f;

void Viewer::mouseMoveEvent ( QMouseEvent * event ) { 
    int deltaX = event->x() - mPreviousX;
    mPreviousX = event->x();
    
    if(deltaX != 0) {
        if(mMovingRight != (deltaX > 0)) {
            mMovingRight = deltaX > 0;
            
            if(!mShiftPressed) {
                mTotalX = deltaX;
                mRotateTimer->start();
            }    
        }
        
        if(!mShiftPressed) {
            if(mRotateTimer->isActive()) {
                mTotalX += deltaX;
            } else {
                mRotateTimer->start();
                mTotalX = deltaX;
            }

            float rotateFactor = deltaX / ROTATE_DIVISOR;

            if((mButtonsPressed & Qt::LeftButton) > 0) {
                rotateWorld(rotateFactor, 1, 0, 0);
            }

            if((mButtonsPressed & Qt::RightButton) > 0) {
                rotateWorld(rotateFactor, 0, 1, 0);
            }

            if((mButtonsPressed & Qt::MiddleButton) > 0) {
                rotateWorld(rotateFactor, 0, 0, 1);
            }
        
        } else {
            float scaleFactor = 1 + deltaX/SCALE_DIVISOR;

            if(mTotalScaling * scaleFactor > mScaleMax) {
                scaleFactor = mScaleMax/mTotalScaling;

            } else if(mTotalScaling * scaleFactor < mScaleMin) {
                scaleFactor = mScaleMin/mTotalScaling;
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

void Viewer::persistenceRotate() {
    mTransformMatrix.rotate(mPersistenceSpeed/ROTATE_DIVISOR, mPersistenceAxis);
}
