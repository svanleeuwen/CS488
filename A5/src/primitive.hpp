#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class Primitive {
public:
    virtual ~Primitive();

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) = 0;
};

class Sphere : public Primitive {
public:

    virtual ~Sphere();

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object);
};

class Cube : public Primitive {
public:
    virtual ~Cube();

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object);
};

class NonhierSphere : public Primitive {
public:
    NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
    {
    }
    virtual ~NonhierSphere();

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object);

private:
    Point3D m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
public:
    NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
    {
    }

    virtual ~NonhierBox();

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object);

private:
    Point3D m_pos;
    double m_size;
};

#endif