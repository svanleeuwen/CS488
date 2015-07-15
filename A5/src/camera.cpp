#include "camera.hpp"
#include <math.h>

using std::cout;
using std::endl;

Camera::Camera(int width, int height, const Point3D& eye, const Vector3D& view, const Vector3D& up, double fov):
    m_width(width), m_height(height), m_eye(eye), m_view(view), m_up(up), m_fov(fov) 
{
    updateScreenToWorld();
}

void Camera::copy(const Camera& other) {
    m_width = other.m_width;
    m_height = other.m_height;
    
    m_eye = other.m_eye;
    m_view = other.m_view;
    m_up = other.m_up;

    m_fov = other.m_fov;

    m_screenToWorld = other.m_screenToWorld;
}

Camera::Camera(const Camera& other) {
    copy(other);
}

Camera& Camera::operator=(const Camera& other) {
    if(this != &other) {
        copy(other);
    }

    return *this;
}

Ray Camera::getRay(double xPos, double yPos) const{
    Point3D pk(xPos, yPos, 0.0);

    Point3D pWorld = m_screenToWorld * pk;

    return Ray(m_eye, pWorld - m_eye);
}

void Camera::updateDimensions(int width, int height) {
    m_width = width;
    m_height = height;

    updateScreenToWorld();
}

void Camera::updateScreenToWorld() {
    // Build the transformation from June 17 lecture
    Matrix4x4 T1 = Matrix4x4::getTransMat(Vector3D(-m_width/2.0, -m_height/2.0, 1.0));

    double h = 2.0 * tan(m_fov * M_PI / 360.0);
    Matrix4x4 S2 = Matrix4x4::getScaleMat(Vector3D(-h/m_height, -h/m_height, 1.0));

    Matrix4x4 R3 = getViewToWorld();

    Matrix4x4 T4 = Matrix4x4::getTransMat(Vector3D(m_eye)); 

    m_screenToWorld = T4 * R3 * S2 * T1;
}

Matrix4x4 Camera::getViewToWorld() {
    Vector3D w = m_view;
    w.normalize();

    Vector3D u = cross(m_up, w);
    u.normalize();

    Vector3D v = cross(w, u);

    Matrix4x4 R;

    R[0][0] = u[0];
    R[1][0] = u[1];
    R[2][0] = u[2];

    R[0][1] = v[0];
    R[1][1] = v[1];
    R[2][1] = v[2];

    R[0][2] = w[0];
    R[1][2] = w[1];
    R[2][2] = w[2];

    return R;
}
