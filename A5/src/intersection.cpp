#include "intersection.hpp"
#include "scene.hpp"
#include <iostream>

using std::cout;
using std::endl;

Intersection::Intersection(const Point3D& point, double t, Primitive* primitive, const Vector3D& normal):
    m_point(point), m_param(t), m_primitive(primitive), m_normal(normal)
{
}

Intersection::~Intersection() {}

Intersection::Intersection(const Intersection& other) {
    m_point = other.m_point;
    m_param = other.m_param;

    m_primitive = other.m_primitive;
    m_normal = other.m_normal;
}

Intersection& Intersection::operator=(const Intersection& other) {
    if(&other != this) {
        m_point = other.m_point;
        m_param = other.m_param;

        m_primitive = other.m_primitive;
        m_normal = other.m_normal;
    }
    return *this;
}

Vector3D Intersection::getNormal() const {
    if(m_primitive->getBump() == NULL) {
        return m_normal;
    }

    Vector3D offset = m_primitive->getOffset(m_point);
    return m_normal + offset;
}

Colour Intersection::getDiffuse() const {
    return m_primitive->getColour(m_point);    
}

Colour Intersection::getSpecular() const {
    PhongMaterial* material = m_primitive->getMaterial();
    return material->getKS();
}

Colour Intersection::getShininess() const {
    PhongMaterial* material = m_primitive->getMaterial();
    return material->getShininess();
}

std::ostream& operator<<(std::ostream& out, const Intersection& isect)
{
    out << "I[" << isect.getPoint() << ", " << isect.getParam() << ", "
        << isect.getNormal();
    out << "]";
    return out;
}
