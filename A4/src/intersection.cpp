#include "intersection.hpp"
#include "scene.hpp"
#include <iostream>

using std::cout;
using std::endl;

Intersection::Intersection(const Point3D& point, double t, GeometryNode* object, Vector3D& normal):
    m_point(point), m_param(t), m_object(object), m_normal(normal)
{
}

Intersection::~Intersection() {}

Intersection::Intersection(const Intersection& other) {
    m_point = other.m_point;
    m_param = other.m_param;
    m_object = other.m_object;
    m_normal = other.m_normal;
}

Intersection& Intersection::operator=(const Intersection& other) {
    if(&other != this) {
        m_point = other.m_point;
        m_object = other.m_object;
        m_param = other.m_param;
        m_normal = other.m_normal;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, const Intersection& isect)
{
    out << "I[" << isect.getPoint() << ", " << isect.getParam() << ", "
        << isect.getNormal();
    out << "]";
    return out;
}
