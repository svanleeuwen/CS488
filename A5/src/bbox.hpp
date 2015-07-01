#ifndef CS488_BBOX_HPP
#define CS488_BBOX_HPP

#include "ray.hpp"
#include "algebra.hpp"

class BBox {
public:
    virtual ~BBox();

    virtual bool intersect(const Ray& ray) = 0;
};

class AABB : public BBox {
public:
    AABB() {}
    AABB(Point3D min, Point3D max);
    virtual ~AABB();

    AABB(const AABB& other);
    AABB& operator=(const AABB& other);

    virtual bool intersect(const Ray& ray);

    static AABB getTransform(const AABB& bbox, const Matrix4x4& trans);
    
    Point3D m_min;
    Point3D m_max;
};

#endif
