#include "ray.hpp"
#include <iostream>
#include <limits>

using std::cout;
using std::endl;

Ray::Ray(Point3D origin, Vector3D direction, double epsilon) {
    m_origin = origin;

    m_direction = direction;
    m_direction.normalize();

    m_hasEndpoint = false;
    m_endpoint = m_origin + 10*m_direction;

    m_epsilon = epsilon;
    m_length = std::numeric_limits<double>::infinity();
}

Ray::Ray(Point3D origin, Point3D endpoint, double epsilon) {
    m_origin = origin;

    m_direction = endpoint - origin;
    m_length = m_direction.length();

    m_direction.normalize();

    m_hasEndpoint = true;
    m_endpoint = endpoint;

    m_epsilon = epsilon;
}

void Ray::copy(const Ray& other) {
    m_origin = other.m_origin;
    m_direction = other.m_direction;

    m_hasEndpoint = other.m_hasEndpoint;
    m_endpoint = other.m_endpoint;

    m_epsilon = other.m_epsilon;
    m_length = other.m_length;
}

Ray::Ray(const Ray& other) {
    copy(other);
}   

Ray& Ray::operator=(const Ray& other) {
    if(&other != this) {
        copy(other);        
    }

    return *this;
}

bool Ray::checkParam(double t) const {
    if(t <= 0 || ((*this)(t) - m_origin).length() < m_epsilon) {
        return false;
    } else if(m_hasEndpoint && t < m_length) {
        return true;
    } else if(!m_hasEndpoint) {
        return true;
    }

    return false;
}

Ray Ray::getTransform(Matrix4x4& trans) const {
    Ray r;

    r.m_origin = trans * m_origin;
    r.m_hasEndpoint = m_hasEndpoint;

    r.m_endpoint = trans * m_endpoint;
    r.m_direction = r.m_endpoint - r.m_origin;

    if(m_hasEndpoint) {
        r.m_length = (r.m_direction).length();
    } else {
        r.m_length = m_length;
    }

    (r.m_direction).normalize();
    r.m_epsilon = m_epsilon;

    return r;
}
