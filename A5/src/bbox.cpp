#include "bbox.hpp"

AABB::AABB(Point3D min, Point3D max) {
    m_min = min;
    m_max = max;

    m_ibbox = IVector3D(min);
    m_ibbox.extend(max);
}

AABB::~AABB() {}

AABB::AABB(const AABB& other) {
    m_min = other.m_min;
    m_max = other.m_max;

    m_ibbox = other.m_ibbox;
}

AABB& AABB::operator=(const AABB& other) {
    if(&other != this) {
        m_min = other.m_min;
        m_max = other.m_max;

        m_ibbox = other.m_ibbox;
    }

    return *this;
}

AABB AABB::getTransform(const AABB& bbox, const Matrix4x4& trans) {
    Vector4D m1 = trans.getColumn(0);
    Vector4D m2 = trans.getColumn(1);
    Vector4D m3 = trans.getColumn(2);
    Vector4D m4 = trans.getColumn(3);

    Point3D xa = Point3D(m1 * bbox.m_min[0]);
    Point3D xb = Point3D(m1 * bbox.m_max[0]);

    Point3D ya = Point3D(m2 * bbox.m_min[1]);
    Point3D yb = Point3D(m2 * bbox.m_max[1]);

    Point3D za = Point3D(m3 * bbox.m_min[2]);
    Point3D zb = Point3D(m3 * bbox.m_max[2]);

    Point3D t = Point3D(m4);

    Point3D min = Point3D::min(xa, xb) + Point3D::min(ya, yb) + Point3D::min(za, zb) + t;
    Point3D max = Point3D::max(xa, xb) + Point3D::max(ya, yb) + Point3D::max(za, zb) + t;

    return AABB(min, max);
}

bool AABB::packetTest(const Packet& packet) {
    IVector3D direction = packet.getDirection();
    IVector3D ratio = packet.getRatio();

    bool is_finite = packet.isFinite();
    Interval ilength = Interval(0, packet.getLength());

    IVector3D T = (m_ibbox * direction.reciprocal()) - ratio;

    if(is_finite) {
        return !Interval::set_intersection(T.intersectMembers(), ilength).isEmpty();
    } else {
        return !T.intersectMembers().isEmpty();
    }
}

bool AABB::intersect(const Ray& ray) const{
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    Vector3D p_min = m_min - ray.getOrigin();
    Vector3D p_max = m_max - ray.getOrigin();

    Vector3D d = ray.getDirection();
    bool finite_ray = ray.hasEndpoint();
    double ray_length = ray.getLength();

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

        if(finite_ray && t_min > ray_length) {
            return false;
        }
    }

    if(finite_ray && t_min < ray.getEpsilon() && t_max > ray_length) {
        return false;
    }

    return true;
}

bool AABB::contains(const Ray& ray) const{
    Point3D origin = ray.getOrigin();

    bool inside = true;

    inside = inside && origin[0] <= m_max[0] + 1.0e-10;
    inside = inside && origin[1] <= m_max[1] + 1.0e-10;
    inside = inside && origin[2] <= m_max[2] + 1.0e-10;

    inside = inside && origin[0] >= m_min[0] - 1.0e-10;
    inside = inside && origin[1] >= m_min[1] - 1.0e-10;
    inside = inside && origin[2] >= m_min[2] - 1.0e-10;

    return inside;
}
