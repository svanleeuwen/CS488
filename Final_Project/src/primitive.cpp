#include "primitive.hpp"
#include "polyroots.hpp"

#include <math.h>
#include <iostream>
#include <limits>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

Primitive::~Primitive()
{
}

Primitive::Primitive(const Primitive& other) {
    m_trans = other.m_trans;
    m_inv = other.m_inv;

    m_modelBBox = other.m_modelBBox;
    m_worldBBox = other.m_worldBBox;

    m_material = other.m_material;
    m_texture = other.m_texture;
    m_bump = other.m_bump;
}

Primitive& Primitive::operator=(const Primitive& other) {
    if(&other != this) {
        m_trans = other.m_trans;
        m_inv = other.m_inv;

        m_modelBBox = other.m_modelBBox;   
        m_worldBBox = other.m_worldBBox;

        m_material = other.m_material;
    }
    
    return *this;
}

void Primitive::setTransform(const Matrix4x4& trans, const Matrix4x4& inv) {
    m_trans = trans;
    m_inv = inv;

    m_worldBBox = AABB::getTransform(m_modelBBox, trans);
}

void Primitive::setBBox(const Point3D& min, const Point3D& max) {
    m_modelBBox = AABB(min, max);
    m_worldBBox = AABB(min, max);
}

bool Primitive::allMiss(const Packet& packet) {
    return m_worldBBox.allMiss(packet);
}

void Primitive::getIntersection(Packet& packet, int firstActive, vector<bool>& v_hit, vector<Intersection>* v_isect) {
/*    if(allMiss(packet)) {
        return;
    }*/

    vector<Ray*>* rays = packet.getRays();
    int n = rays->size();

    for(int i = firstActive; i < n; ++i) {
        Ray* ray = rays->at(i);

        if(ray != NULL) {
            Intersection* isect = v_isect == NULL ? NULL : &v_isect->at(i);

            if(isect != NULL || !v_hit.at(i)) {
                bool hit = getIntersection(*ray, isect);

                if(hit) {
                    v_hit.at(i) = true;

                    if(isect != NULL) {
                        Ray* oldRay = rays->at(i);
                        rays->at(i) = new Ray(ray->getOrigin(), isect->getPoint());

                        delete oldRay;
                    }
                }
            }
        }
    }
}

Colour Primitive::getColour(const Point3D& point) {
    (void)point;

    return m_material->getKD();
}

Vector3D Primitive::getOffset(const Point3D& point) {
    (void)point;

    return Vector3D(0, 0, 0);
}

Sphere::Sphere() {
    Point3D min(-1, -1, -1);
    Point3D max(1, 1, 1);

    setBBox(min, max);
}

Sphere::~Sphere()
{
}

Sphere::Sphere(const Sphere& other) : Primitive(other) {}

Sphere& Sphere::operator=(const Sphere& other) {
    Primitive::operator=(other);
    return *this;
}

bool Sphere::getIntersection(const Ray& ray, Intersection* isect) {
    Ray modelRay = ray.getTransform(m_inv);

    if(!m_modelBBox.intersect(modelRay)) {
        return false;
    } 

    Vector3D o = Vector3D(modelRay.getOrigin());
    Vector3D d = modelRay.getDirection();

    double A = d.length2();
    double B = 2*d.dot(o);
    double C = o.length2() - 1.0;

    double roots[2];
    int numRoots = quadraticRoots(A, B, C, roots);

    bool hit = false;
    double t;

    if(numRoots == 1 && modelRay.checkParam(roots[0])) {
        t = roots[0];
        hit = true;

    } else if(numRoots == 2) {
        if(modelRay.checkParam(roots[0]) && modelRay.checkParam(roots[1])) {
            t = fmin(roots[0], roots[1]);
            hit = true;

        } else if(modelRay.checkParam(roots[0])) {
            t = roots[0];
            hit = true;

        } else if(modelRay.checkParam(roots[1])) {
            t = roots[1];
            hit = true;
        }
    }

    if(hit && isect != NULL) {
        Point3D point = modelRay(t);
        Vector3D normal = transNorm(m_inv, Vector3D(point));
        normal.normalize();

        point = m_trans * point;
        *isect = Intersection(point, t, this, normal);
    }

    return hit;
}

Cube::Cube() {
    Point3D min(0, 0, 0);
    Point3D max(1, 1, 1);

    setBBox(min, max);
}

Cube::~Cube()
{
}

Cube::Cube(const Cube& other) : Primitive(other) {}

Cube& Cube::operator=(const Cube& other) {
    Primitive::operator=(other);
    return *this;
}

bool Cube::getIntersection(const Ray& ray, Intersection* isect) {
    Ray modelRay = ray.getTransform(m_inv);

    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    Vector3D p_min = - Vector3D(modelRay.getOrigin());
    Vector3D p_max = Point3D(1.0, 1.0, 1.0) - modelRay.getOrigin();

    Vector3D normal_min;
    Vector3D normal_max;

    Vector3D d = modelRay.getDirection();
    bool finite_ray = modelRay.hasEndpoint();
    
    double ray_length = modelRay.getLength();
    double epsilon = modelRay.getEpsilon();
    
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

            if(t_min > t_max || t_max < epsilon) {
                return false;
            }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > ray_length){
            return false;
        }
    } 

    if(finite_ray && t_min <= epsilon && t_max > ray_length) {
        return false;
    }

    if(isect != NULL) {
        if(t_min > epsilon) {
            Point3D point = m_trans * modelRay(t_min);
            Vector3D normal = transNorm(m_inv, normal_min);
            normal.normalize();

            *isect = Intersection(point, t_min, this, normal);

        } else {
            Point3D point = m_trans * modelRay(t_max);
            Vector3D normal = transNorm(m_inv, normal_max);
            normal.normalize();

            *isect = Intersection(point, t_max, this, normal);
        }
    }

    return true;
}

int getDropIndex(const Point3D& point) {
    int index = 0;
    double min = fabs(point[0]);

    for(int i = 1; i < 3; ++i) {
        double val = fabs(point[i]);

        if(val < min) {
            index = i;
            min = val;
        }
        if(1.0 - val < min) {
            index = i;
            min = 1.0 - val;
        }
    }

    return index;
}

Colour Cube::getColour(const Point3D& point) {
    if(m_texture == NULL) {
        return m_material->getKD();
    }
    
    Point3D modelPoint = m_inv * point;
    int index = getDropIndex(modelPoint);

    return m_texture->getColour(modelPoint.dropDim(index));
}

Vector3D Cube::getOffset(const Point3D& point) {
    Vector3D offset3D(0, 0, 0);

    Point3D modelPoint = m_inv * point;
    int index = getDropIndex(modelPoint);

    Point2D offset2D = m_bump->getOffset(modelPoint.dropDim(index));

    int i = 0;
    int j = 0;

    while(j < 3) {
        if(j == index) {
            ++j;
        } else {
            offset3D[j++] = offset2D[i++];
        }
    }

    if(modelPoint[index] > 0.5) {
        offset3D = -offset3D;
    }

    return transNorm(m_trans, offset3D);
}

NonhierSphere::NonhierSphere(const Point3D& pos, double radius) :
    m_pos(pos), m_radius(radius) 
{
    Point3D min(m_pos[0] - m_radius, m_pos[1] - m_radius, m_pos[2] - m_radius);
    Point3D max(m_pos[0] + m_radius, m_pos[1] + m_radius, m_pos[2] + m_radius);

    setBBox(min, max);   
}

NonhierSphere::~NonhierSphere()
{
}


NonhierSphere::NonhierSphere(const NonhierSphere& other) : Primitive(other)
{
    m_pos = other.m_pos;
    m_radius = other.m_radius;
}

NonhierSphere& NonhierSphere::operator=(const NonhierSphere& other) {
    if(&other != this) {
        Primitive::operator=(other);

        m_pos = other.m_pos;
        m_radius = other.m_radius;
    }
    return *this;
}

bool NonhierSphere::getIntersection(const Ray& ray, Intersection* isect) {
    Ray modelRay = ray.getTransform(m_inv);

    if(!m_modelBBox.intersect(modelRay)) {
        return false;
    }

    Point3D o = modelRay.getOrigin();
    Vector3D d = modelRay.getDirection();

    double A = d.length2();
    double B = 2*d.dot(o - m_pos);
    double C = (o - m_pos).length2() - m_radius*m_radius;

    double roots[2];
    int numRoots = quadraticRoots(A, B, C, roots);

    bool hit = false;
    double t;

    if(numRoots == 1 && modelRay.checkParam(roots[0])) {
        t = roots[0];
        hit = true;

    } else if(numRoots == 2) {
        if(modelRay.checkParam(roots[0]) && modelRay.checkParam(roots[1])) {
            t = fmin(roots[0], roots[1]);
            hit = true;

        } else if(modelRay.checkParam(roots[0])) {
            t = roots[0];
            hit = true;

        } else if(modelRay.checkParam(roots[1])) {
            t = roots[1];
            hit = true;
        }
    }

    if(hit && isect != NULL) {
        Point3D point = modelRay(t);
        Vector3D normal = transNorm(m_inv, (point - m_pos));
        normal.normalize();

        point = m_trans * point;
        *isect = Intersection(point, t, this, normal);
    }

    return hit;
}

NonhierBox::NonhierBox(const Point3D& pos, double size) :
    m_pos(pos), m_size(size)
{
    Point3D max = m_pos + Point3D(m_size, m_size, m_size);
    setBBox(m_pos, max);
}

NonhierBox::~NonhierBox()
{
}

NonhierBox::NonhierBox(const NonhierBox& other) : Primitive(other)
{
    m_pos = other.m_pos;
    m_size = other.m_size;
}

NonhierBox& NonhierBox::operator=(const NonhierBox& other) {
    if(&other != this) {
        Primitive::operator=(other);

        m_pos = other.m_pos;
        m_size = other.m_size;
    }

    return *this;
}

bool NonhierBox::getIntersection(const Ray& ray, Intersection* isect) {
    Ray modelRay = ray.getTransform(m_inv);

    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();
 
    Vector3D p_min = m_pos - modelRay.getOrigin();
    Vector3D p_max = Point3D(m_pos[0] + m_size, m_pos[1] + m_size, m_pos[2] + m_size) - modelRay.getOrigin();

    Vector3D normal_min;
    Vector3D normal_max;

    Vector3D d = modelRay.getDirection();
    bool finite_ray = modelRay.hasEndpoint();

    double ray_length = modelRay.getLength();
    double epsilon = modelRay.getEpsilon();
    
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

            if(t_min > t_max || t_max < epsilon) {
                return false;
            }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > ray_length) {
            return false;
        }
    }

    if(finite_ray && t_min <= epsilon && t_max > ray_length) {
        return false;
    }  

    if(isect != NULL) { 
        if(t_min > epsilon) {
            Point3D point = m_trans * modelRay(t_min);
            Vector3D normal = transNorm(m_inv, normal_min);
            normal.normalize();

            *isect = Intersection(point, t_min, this, normal);

        } else {
            Point3D point = m_trans * modelRay(t_max);
            Vector3D normal = transNorm(m_inv, normal_max);
            normal.normalize();

            *isect = Intersection(point, t_max, this, normal);
        }
    }

    return true;
}
