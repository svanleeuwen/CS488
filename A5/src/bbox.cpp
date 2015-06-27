#include "bbox.hpp"

BBox::~BBox() {}

AABB::AABB(Point3D min, Point3D max) {
    m_min = min;
    m_max = max;
}

AABB::~AABB() {}

AABB::AABB(const AABB& other) {
    m_min = other.m_min;
    m_max = other.m_max;
}

AABB& AABB::operator=(const AABB& other) {
    m_min = other.m_min;
    m_max = other.m_max;

    return *this;
}

bool AABB::intersect(const Ray& ray) {
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    Vector3D p_min = m_min - ray.getOrigin();
    Vector3D p_max = m_max - ray.getOrigin();

    Vector3D d = ray.getDirection();
    bool finite_ray = ray.hasEndpoint();

    for(int i = 0; i < 3; i++) {
        if(fabs(d[i]) > 1.0e-15) {
            double t1 = p_max[i] / d[i];
            double t2 = p_min[i] / d[i];

            if(t1 < t2) {
                if(t1 > t_min) {
                    t_min = t1;
                }
                if(t2 < t_max) {
                    t_max = t2;
                }
            } else {
                if(t2 > t_min) {
                    t_min = t2;
                }
                if(t1 < t_max) {
                    t_max = t1;
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

    return true;
}
