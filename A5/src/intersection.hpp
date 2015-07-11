#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "algebra.hpp"

class PhongMaterial;

class Intersection {
public:
    Intersection() {};
    Intersection(const Point3D& point, double t, PhongMaterial* material, const Vector3D& normal);

    ~Intersection();

    Intersection(const Intersection& other);
    Intersection& operator=(const Intersection& other);

    Point3D getPoint() const { return m_point; }
    double getParam() const { return m_param; }
    
    PhongMaterial* getMaterial() const { return m_material; }
    Vector3D getNormal() const { return m_normal; }

private:
    Point3D m_point;
    double m_param;

    PhongMaterial* m_material;
    Vector3D m_normal;
};

std::ostream& operator<<(std::ostream& out, const Intersection& isect);
#endif
