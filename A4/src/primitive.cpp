#include "primitive.hpp"
#include "polyroots.hpp"
#include <math.h>
#include <iostream>
#include <limits>

using std::cout;
using std::endl;

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}
bool Sphere::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) {
    (void) ray;
    (void) isect;
    (void) object;
    return false;
}

Cube::~Cube()
{
}
bool Cube::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) {
     (void) ray;
    (void) isect;
    (void) object;
    return false;
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) {
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

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) {
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

            Vector3D normal = Vector3D(0, 0, 0);
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

            if(t_min > t_max || t_max < 0.0) {
                return false;
            }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > 1) {
            return false;
        }
    } 

    if(t_min > 0) {
        *isect = Intersection(ray(t_min), t_min, object, normal_min);
    } else {
        *isect = Intersection(ray(t_max), t_max, object, normal_max);
    }

    return true;
}
