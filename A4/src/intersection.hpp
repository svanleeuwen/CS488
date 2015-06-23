#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "algebra.hpp"

class GeometryNode;

class Intersection {
public:
    Intersection() {};
    Intersection(const Point3D& point, double t, GeometryNode* object, Vector3D& normal);

    ~Intersection();

    Intersection(const Intersection& other);
    Intersection& operator=(const Intersection& other);

    Point3D getPoint() const { return m_point; }
    double getParam() const { return m_param; }
    GeometryNode* getObject() const { return m_object; }
    Vector3D getNormal() const { return m_normal; }

private:
    Point3D m_point;
    double m_param;
    GeometryNode* m_object;
    Vector3D m_normal;
};

std::ostream& operator<<(std::ostream& out, const Intersection& isect);
#endif
