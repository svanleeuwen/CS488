#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "algebra.hpp"
#include "ray.hpp"

class Camera {
   
public:
    Camera(int width, int height, const Point3D& eye, const Vector3D& view, const Vector3D& up, double fov);

    Ray* getRay(double xPos, double yPos) const;

    Point3D getEye() const { return m_eye; }

private:
    Matrix4x4 getViewToWorld();

    int m_width;
    int m_height;

    Point3D m_eye;
    Vector3D m_view;
    Vector3D m_up;

    double m_fov;

    Matrix4x4 m_screenToWorld;
};
#endif
