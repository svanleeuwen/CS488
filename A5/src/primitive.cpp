#include "primitive.hpp"
#include "polyroots.hpp"
#include <math.h>
#include <iostream>
#include <limits>
#include "scene.hpp"

using std::cout;
using std::endl;

Primitive::Primitive(const Point3D& min, const Point3D& max) :
    m_bbox(min, max);
{}

Primitive::~Primitive()
{
}

Primitive(const Primitive& other) {
    m_trans = other.m_trans;
    m_inv = other.m_inv;
    m_bbox = other.m_bbox;
}

Primitive& operator=(const Primitive& other) {
    m_trans = other.m_trans;
    m_inv = other.m_inv;
    m_bbox = other.m_bbox;   
    
    return *this;
}

void Primitive::setTransform(const Matrix4x4& trans, const Matrix4x4& inv) {
    m_trans = trans;
    m_inv = inv;

    m_bbox.transform(trans);
}

Sphere::Sphere() {
    Point3D min(-1, -1, -1);
    Point3D max(1, 1, 1);

    Primitive(min, max);
}

Sphere::~Sphere()
{
}

Sphere(const Sphere& other) {
    Primitive(other);
}

Sphere& operator=(const Sphere& other) {
    Primive(other);
    return *this;
}

bool Sphere::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object, bool getIsect) {
    if(!m_bbox.intersect(ray)) {
        return false;
    } 

    Vector3D o = Vector3D(ray.getOrigin());
    Vector3D d = ray.getDirection();

    double A = d.length2();
    double B = 2*d.dot(o);
    double C = o.length2() - 1.0;

    double roots[2];
    int numRoots = quadraticRoots(A, B, C, roots);

    bool hit = false;
    double t;

    if(numRoots == 1 && ray.checkParam(roots[0])) {
        t = roots[0];
        hit = true;

    } else if(numRoots == 2) {
        if(ray.checkParam(roots[0]) && ray.checkParam(roots[1])) {
            t = fmin(roots[0], roots[1]);
            hit = true;

        } else if(ray.checkParam(roots[0])) {
            t = roots[0];
            hit = true;

        } else if(ray.checkParam(roots[1])) {
            t = roots[1];
            hit = true;
        }
    }

    if(hit) {
        Point3D point = ray(t);
        Vector3D normal = point;
        *isect = Intersection(point, t, object, normal);
    }

    return hit;
}

Cube::Cube() {
    Point3D min(0, 0, 0);
    Point3D max(1, 1, 1);

    Primitive(min, max);
}

Cube::~Cube()
{
}

Cube(const Cube& other) {
    Primitive(other);
}

Cube& operator=(const Cube& other) {
    Primitive(other);
    return *this;
}

bool Cube::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object, bool getIsect) {
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    Vector3D p_min = - Vector3D(ray.getOrigin());
    Vector3D p_max = Point3D(1.0, 1.0, 1.0) - ray.getOrigin();

    Vector3D normal_min;
    Vector3D normal_max;

    Vector3D d = ray.getDirection();
    bool finite_ray = ray.hasEndpoint();
    
    for(int i = 0; i < 3; i++) {
        if(fabs(d[i]) > 1.0e-15) {
            double t1 = p_max[i] / d[i];
            double t2 = p_min[i] / d[i];

            Vector3D normal = Vector3D(0.0, 0.0, 0.0);
            normal[i] = -d[i];

            if(t1 < t2) {
                if(t1 > t_min) {
                    t_min = t1;
                    normal_min = normal;
                }
                if(t2 < t_max) {
                    t_max = t2;
                    normal_max = normal;
                }
            } else {
                if(t2 > t_min) {
                    t_min = t2;
                    normal_min = normal;
                }
                if(t1 < t_max) {
                    t_max = t1;
                    normal_max = normal;
                }
            }

            if(t_min > t_max || t_max < ray.getEpsilon()) {
                return false;
            }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > 1) {
            return false;
        }
    } 

    if(t_min > ray.getEpsilon()) {
        *isect = Intersection(ray(t_min), t_min, object, normal_min);
    } else {
        *isect = Intersection(ray(t_max), t_max, object, normal_max);
    }

    return true;
}

NonhierSphere::NonhierSphere(const Point3D& pos, double radius) :
    m_pos(pos), m_radius(radius) 
{
    Point3D min(m_pos[0] - m_radius, m_pos[1] - m_radius, m_pos[2] - m_radius);
    Point3D max(m_pos[0] + m_radius, m_pos[1] + m_radius, m_pos[2] + m_radius);

    Primitive(min, max);   
}

NonhierSphere::~NonhierSphere()
{
}


NonhierSphere(const NonhierSphere& other) {
    m_pos = other.m_pos;
    m_radius = other.m_radius;
    
    Primitive(other);
}

NonhierSphere& operator=(const NonhierSphere& other) {
    m_pos = other.m_pos;
    m_radius = other.m_radius;
    
    Primitive(other);
    return *this;
}

bool NonhierSphere::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object, bool getIsect) {
    if(!m_bbox.intersect(ray)) {
        return false;
    }

    Point3D o = ray.getOrigin();
    Vector3D d = ray.getDirection();

    double A = d.length2();
    double B = 2*d.dot(o - m_pos);
    double C = (o - m_pos).length2() - m_radius*m_radius;

    double roots[2];
    int numRoots = quadraticRoots(A, B, C, roots);

    bool hit = false;
    double t;

    if(numRoots == 1 && ray.checkParam(roots[0])) {
        t = roots[0];
        hit = true;

    } else if(numRoots == 2) {
        if(ray.checkParam(roots[0]) && ray.checkParam(roots[1])) {
            t = fmin(roots[0], roots[1]);
            hit = true;

        } else if(ray.checkParam(roots[0])) {
            t = roots[0];
            hit = true;

        } else if(ray.checkParam(roots[1])) {
            t = roots[1];
            hit = true;
        }
    }

    if(hit) {
        Point3D point = ray(t);
        Vector3D normal = point - m_pos;
        *isect = Intersection(point, t, object, normal);
    }

    return hit;
}

NonhierBox::NonhierBox() {
    Point3D max = m_pos + Point3D(m_size, m_size, m_size);
    Primitive(m_pos, max);
}

NonhierBox::~NonhierBox()
{
}

NonhierBox(const NonhierBox& other) {
    m_pos = other.m_pos;
    m_size = other.m_size;

    Primitive(other);
}

NonhierBox& operator=(const NonhierBox& other) {
    m_pos = other.m_pos;
    m_size = other.m_size;

    Primitive(other);
    return *this;
}

// Slabs method from Real-time Rendering
bool NonhierBox::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object, bool getIsect) {
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();
 
    Vector3D p_min = m_pos - ray.getOrigin();
    Vector3D p_max = Point3D(m_pos[0] + m_size, m_pos[1] + m_size, m_pos[2] + m_size) - ray.getOrigin();

    Vector3D normal_min;
    Vector3D normal_max;

    Vector3D d = ray.getDirection();
    bool finite_ray = ray.hasEndpoint();
    
    for(int i = 0; i < 3; i++) {
        if(fabs(d[i]) > 1.0e-15) {
            double t1 = p_max[i] / d[i];
            double t2 = p_min[i] / d[i];

            Vector3D normal = Vector3D(0.0, 0.0, 0.0);
            normal[i] = -d[i];

            if(t1 < t2) {
                if(t1 > t_min) {
                    t_min = t1;
                    normal_min = normal;
                }
                if(t2 < t_max) {
                    t_max = t2;
                    normal_max = normal;
                }
            } else {
                if(t2 > t_min) {
                    t_min = t2;
                    normal_min = normal;
                }
                if(t1 < t_max) {
                    t_max = t1;
                    normal_max = normal;
                }
            }

            if(t_min > t_max || t_max < ray.getEpsilon()) {
                return false;
            }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > 1) {
            return false;
        }
    } 

    if(t_min > ray.getEpsilon()) {
        *isect = Intersection(ray(t_min), t_min, object, normal_min);
    } else {
        *isect = Intersection(ray(t_max), t_max, object, normal_max);
    }

    return true;
}
