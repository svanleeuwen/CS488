#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "algebra.hpp"

class Primitive;

class Intersection {
public:
    Intersection() {}
    Intersection(const Point3D& point, double t, Primitive* primitive, const Vector3D& normal);

    ~Intersection();

    Intersection(const Intersection& other);
    Intersection& operator=(const Intersection& other);

    Point3D getPoint() const { return m_point; }
    double getParam() const { return m_param; }
    
    Primitive* getPrimitive() const { return m_primitive; }
    Vector3D getNormal() const { return m_normal; }

    Colour getDiffuse() const;
    Colour getSpecular() const;
    Colour getShininess() const;

private:
    Point3D m_point;
    double m_param;

    Primitive* m_primitive;
    Vector3D m_normal;
};

std::ostream& operator<<(std::ostream& out, const Intersection& isect);
#endif
