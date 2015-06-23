#include "ray.hpp"

Ray::Ray(Point3D origin, Vector3D direction, double epsilon) {
    m_origin = origin;
    m_direction = direction;
    m_hasEndpoint = false;
    m_epsilon = epsilon;
}

Ray::Ray(Point3D origin, Point3D endpoint, double epsilon) {
    m_origin = origin;
    m_direction = endpoint - origin;

    m_hasEndpoint = true;
    m_endpoint = endpoint;

    m_epsilon = epsilon;
}

Ray::Ray(const Ray& other) {
    m_origin = other.m_origin;
    m_direction = other.m_direction;

    m_hasEndpoint = other.m_hasEndpoint;
    m_endpoint = other.m_endpoint;

    m_epsilon = other.m_epsilon;
}   

Ray& Ray::operator=(const Ray& other) {
    if(&other != this) {
        m_origin = other.m_origin;
        m_direction = other.m_direction;
       
        m_hasEndpoint = other.m_hasEndpoint;
        m_endpoint = other.m_endpoint;
      
        m_epsilon = other.m_epsilon;   
    }

    return *this;
}

bool Ray::checkParam(double t) const {
    if(t <= 0 || ((*this)(t) - m_origin).length() < m_epsilon) {
        return false;
    } else if(m_hasEndpoint && t < 1) {
        return true;
    } else if(!m_hasEndpoint) {
        return true;
    }

    return false;
}
