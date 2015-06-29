#ifndef CS488_BBOX_HPP
#define CS488_BBOX_HPP

#include "ray.hpp"
#include "algebra.hpp"

class BBox {
public:
    virtual ~BBox();

    virtual void transform(const Matrix& trans) = 0;
    virtual bool intersect(const Ray& ray) = 0;
};

class AABB : public BBox {
public:
    AABB() {}
    AABB(Point3D min, Point3D max);
    virtual ~AABB();
    AABB(const AABB& other);
    
    AABB& operator=(const AABB& other);

    virtual void transform(const Matrix& trans);
    virtual bool intersect(const Ray& ray);

private:
    Point3D m_min;
    Point3D m_max;
};

#endif
