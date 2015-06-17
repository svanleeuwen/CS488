#include <QtWidgets>
#include <QtOpenGL>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>

#include "scene_lua.hpp"
#include "sphere.hpp"
#include <vector>
#include <QVector3D>
#include "trackball.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

using namespace std;

Viewer::Viewer(const QGLFormat& format, QWidget *parent) 
    : QGLWidget(format, parent) 
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
{
    mPosOriMode = true;
    mTrackingCircle = false;

    mZBuffer = false;
    mBackfaceCulling = false;
    mFrontfaceCulling = false;

    mButtonsPressed = 0;

    initializeScene();

    mMatStack = new std::stack<QMatrix4x4>();
    QMatrix4x4 eye;
    mMatStack->push(eye);
}

Viewer::~Viewer() {
    // Nothing to do here right now.
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 300);
}

float* Viewer::getSphereData() {
    SphereMesh sphere(4);
    vector<Triangle*>* faces = sphere.getFaces();

    int n = faces->size();
    mSphereVertexCount = n*3;
    
    float* sphereData = new float[mSphereVertexCount*3*3];

    for(uint i = 0; i < faces->size(); i++) {
        vector<Point*>& verts = faces->at(i)->getVertices();
       
        for(int j = 0; j < 3; j++) { 
            QVector3D point = (QVector3D)(*verts.at(j));
            point.normalize();

            sphereData[(i*3 + j)*3 + 0] = point.x();
            sphereData[(i*3 + j)*3 + 1] = point.y();
            sphereData[(i*3 + j)*3 + 2] = point.z();
        }
    }

    return sphereData;
}

void Viewer::addShaders() {
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
}

void Viewer::initializeVertexBuffer() {
    float* sphereData = getSphereData();

    float circleData[120];

    double radius = 1.0;

    for(size_t i=0; i<40; ++i) {
        circleData[i*3] = radius * cos(i*2*M_PI/40);
        circleData[i*3 + 1] = radius * sin(i*2*M_PI/40);
        circleData[i*3 + 2] = 0.0;
    }

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mVertexBufferObject.allocate((mSphereVertexCount + 40) * 3 * sizeof(float));
    
    mVertexBufferObject.write(0, sphereData, mSphereVertexCount * 3 * sizeof(float));
    mVertexBufferObject.write(mSphereVertexCount * 3 * sizeof(float), circleData, 40 * 3 * sizeof(float));
    
    delete sphereData;
}

void Viewer::initializeProgram() {
    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
    mColorLocation = mProgram.uniformLocation("diffuse_color");
    mCenterLocation = mProgram.uniformLocation("sphere_center");
    mPickingLocation = mProgram.uniformLocation("picking");
    mLightLocation = mProgram.uniformLocation("light_location");
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glShadeModel(GL_SMOOTH);
    glClearColor( 0.4, 0.4, 0.4, 0.0 );

    addShaders();    

    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    initializeVertexBuffer();

    initializeProgram();
}

void Viewer::drawSphere(bool picking) {
    // Bind buffer objectrot
    mVertexBufferObject.bind();
    mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mMatStack->top());

    QVector4D origin = QVector4D(0, 0, 0, 1);
    mProgram.setUniformValue(mCenterLocation, getCameraMatrix() * mMatStack->top() * origin); 
    mProgram.setUniformValue(mLightLocation, getCameraMatrix() * origin);

    mProgram.setUniformValue(mPickingLocation, picking);

    // Draw buffer
    glDrawArrays(GL_TRIANGLES, 0, mSphereVertexCount);
}rot

void Viewer::paintGL() {
    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mScene->walk_gl(this);
    draw_trackball_circle();
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
    mButtonsPressed += event->button();
    mPreviousX = event->x();
    mPreviousY = event->y();

    if(!mPosOriMode && event->button() == Qt::LeftButton) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // picking method taken from provided tutorial
        mScene->walk_gl(this, true);
        glFlush();
        swapBuffers();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char data[4];
        glReadPixels(event->x(), height() - event->y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        int pickedId = data[0] + data[1]*256 + data[2]*256*256;
        mScene->toggle_picking(pickedId);
        
        update();
    }
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    mButtonsPressed -= event->button();

    if(!mPosOriMode && 
            (event->button() == Qt::RightButton || 
             event->button() == Qt::MiddleButton)) {
        mScene->push_rotation(); 
    }
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    int current_width = width();
    int current_height = height();

    int x = event->x();
    int y = event->y();

    if(mPosOriMode) {
        // The following if statements are modifications to the function vPerformTransfo
        // from events.c
        if (mButtonsPressed & Qt::LeftButton) {
            QMatrix4x4 panMat;
            panMat.translate((x - mPreviousX) / ((float)SENS_PANX),
                -(y - mPreviousY) / ((float)SENS_PANY), 0.0);
            
            mTranslateMatrix = mTranslateMatrix * panMat; 
        }
       
        if (mButtonsPressed & Qt::MiddleButton) {
            QMatrix4x4 zoomMat;
            zoomMat.translate(0.0, 0.0, (y - mPreviousY) / ((float)SENS_ZOOM));

            mTranslateMatrix = mTranslateMatrix * zoomMat;
        }

        if (mButtonsPressed & Qt::RightButton) {
            float fDiameter;
            int iCenterX, iCenterY;
            float fNewModX, fNewModY, fOldModX, fOldModY;
        
            /* vCalcRotVec expects new and old positions in relation to the center of the
             * trackball circle which is centered in the middle of the window and
             * half the smaller of nWinWidth or nWinHeight.
             */
            fDiameter = (current_width < current_height) ? current_width * 0.5 : current_height * 0.5;
            iCenterX = current_width / 2;
            iCenterY = current_height / 2;
            fOldModX = mPreviousX - iCenterX;
            fOldModY = mPreviousY - iCenterY;
            fNewModX = x - iCenterX;
            fNewModY = y - iCenterY;

            QVector3D rotVec = calcRotVec(fNewModX, fNewModY,
                            fOldModX, fOldModY,
                            fDiameter);
            /* Negate Y component since Y axis increases downwards
             * in screen space and upwards in OpenGL.
             */
            QMatrix4x4 rotMat = axisRotMatrix(rotVec);

            // Since all these matrices are meant to be loaded
            // into the OpenGL matrix stack, we need to transpose the
            // rotation matrix (since OpenGL wants rows stored
            // in columns)
            
            mRotateMatrix = mRotateMatrix * rotMat.transposed();
        }
    } else {
        int deltaX = x - mPreviousX;
        int deltaY = y - mPreviousY;

        double xRatio = 0.0;
        double yRatio = 0.0;
        
        if(mButtonsPressed & Qt::MiddleButton) {
            yRatio = (double)deltaY / height();
        }

        if(mButtonsPressed & Qt::RightButton) {
            xRatio = (double)deltaX / width();
        }

        mScene->rotate_picked_nodes(yRatio * 180.0, xRatio * 180.0);
    }

    mPreviousX = x;
    mPreviousY = y;
    
    update();
}

QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 40.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTranslateMatrix * mRotateMatrix;
}

void Viewer::set_colour(const QColor& col)
{
    mProgram.setUniformValue(mColorLocation, col.redF(), col.greenF(), col.blueF());
}

void Viewer::draw_trackball_circle()
{
    int current_width = width();
    int current_height = height();

    float radius = min(current_width, current_height)/4.0;

    if(mTrackingCircle) {
        set_colour(QColor(0.0, 0.0, 0.0));

        // Set orthographic Matrix
        QMatrix4x4 orthoMatrix;
        orthoMatrix.ortho(0.0, (float)current_width, 
               0.0, (float)current_height, -0.1, 0.1);

        // Translate the view to the middle
        QMatrix4x4 transformMatrix;
        transformMatrix.translate(current_width/2.0, current_height/2.0, 0.0);
        transformMatrix.scale(radius, radius, 1.0);
        
        // Bind buffer object
        mVertexBufferObject.bind();
        mProgram.setUniformValue(mMvpMatrixLocation, orthoMatrix * transformMatrix);

        mProgram.setUniformValue(mPickingLocation, true);

        // Draw buffer
        glDrawArrays(GL_LINE_LOOP, mSphereVertexCount, 40);    
    }
}

void Viewer::initializeScene() {
    if(QCoreApplication::arguments().size() > 1) {
        mScene = import_lua(QCoreApplication::arguments().at(1).toUtf8().constData());
    } else {
        mScene = import_lua("puppet.lua");
    }
}

void Viewer::resetPos() {
    mTranslateMatrix.setToIdentity();
    update();
}

void Viewer::resetOri() {
    mRotateMatrix.setToIdentity();
    update();
}

void Viewer::resetJoints() {
    mScene->reset_joints();
    update();
}

void Viewer::undo() {
    bool undone = mScene->undo();
    if(!undone) {
        cout << "No actions left to undo" << endl;
    }

    update();
}

void Viewer::redo() {
    bool redone = mScene->redo();
    if(!redone) {
        cout << "No actions left to redo" << endl;
    }

    update();
}

void Viewer::toggleTrackingCircle() {
    mTrackingCircle = !mTrackingCircle;
    update();
}

void Viewer::toggleZBuffer() {
    mZBuffer = !mZBuffer;
    
    if(mZBuffer) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    update();
}

void Viewer::updateCulling() {
    if(mBackfaceCulling && mFrontfaceCulling) {
        glCullFace(GL_FRONT_AND_BACK);
        glEnable(GL_CULL_FACE);
    
    } else if(mBackfaceCulling) {
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

    } else if(mFrontfaceCulling) {
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);

    } else {
        glDisable(GL_CULL_FACE);
    }

    update();
}

void Viewer::toggleBackface() {
    mBackfaceCulling = !mBackfaceCulling;
    updateCulling();    
}

void Viewer::toggleFrontface() {
    mFrontfaceCulling = !mFrontfaceCulling;
    updateCulling();
}
