#ifndef RAY_HPP
#define RAY_HPP

#include "algebra.hpp"

class Ray {

public:
    Ray() {}

    Ray(Point3D origin, Vector3D direction, double epsilon = 0.000001); 
    Ray(Point3D origin, Point3D endpoint, double epsilon = 0.000001); 
    
    ~Ray() {}
    Ray(const Ray& other); 
    
    Ray& operator=(const Ray& other);

    Point3D operator() (double t) const { return m_origin + t * m_direction; }

    const Point3D& getOrigin() const { return m_origin; }
    const Vector3D& getDirection() const { return m_direction; }

    bool hasEndpoint() const { return m_hasEndpoint; }

    bool checkParam(double t) const;

private:
    Point3D m_origin;
    Vector3D m_direction;

    bool m_hasEndpoint;
    Point3D m_endpoint;

    double m_epsilon;
};

inline Ray operator*(const Matrix4x4& mat, const Ray& ray) {
    return Ray(mat * ray.getOrigin(), mat*ray.getDirection());
}

#endif
