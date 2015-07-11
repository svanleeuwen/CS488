#ifndef CS488_BBOX_HPP
#define CS488_BBOX_HPP

#include "ray.hpp"
#include "algebra.hpp"

class AABB{
public:
    AABB() {}
    AABB(Point3D min, Point3D max);
    ~AABB();

    AABB(const AABB& other);
    AABB& operator=(const AABB& other);

    bool intersect(const Ray& ray) const;
    bool contains(const Ray& ray) const;

    double getMedian(int axis) const;

    static AABB getTransform(const AABB& bbox, const Matrix4x4& trans);
    
    Point3D m_min;
    Point3D m_max;
};

inline double AABB::getMedian(int axis) const {
    return (m_max[axis] + m_min[axis]) / 2.0;
}

#endif
