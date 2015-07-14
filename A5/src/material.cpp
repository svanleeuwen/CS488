#include "material.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double transmitRatio, double medium)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
    m_transmitRatio = transmitRatio;
    m_medium = medium;
}

PhongMaterial::~PhongMaterial()
{
}

static double clamp(double val, double min, double max) {
    if(val > max) {
        return max;
    }else if(val < min) {
        return min;
    } else {
        return val;
    }
}

/*  getColour 
 *  
 *  Parameters: 
 *      l: the vector from the surface towards the light 
 *      v: the vector from the surface towards the camera 
 *      isect: the point where the light intersects the surface
 *      light: the light source 
 */
Colour PhongMaterial::getColour(const Vector3D& l, const Vector3D& v, const Intersection* isect, const Light& light) {
    Colour colour(0.0, 0.0, 0.0);

    Vector3D l_norm = l;
    Vector3D v_norm = v;

    Vector3D normal = isect->getNormal();

    if(v_norm.dot(normal) < 0) {
        normal = -normal;
    }

    Point3D point = isect->getPoint();
    Colour intensity = light.getIntensity(point);

    colour += m_kd * intensity * clamp(l_norm.dot(normal), 0.0, 1.0);

    Vector3D r = - l_norm + 2 * normal * l_norm.dot(normal);
    colour += m_ks * pow(clamp(r.dot(v_norm), 0.0, 1.0), m_shininess) * intensity;

    return colour;
}
