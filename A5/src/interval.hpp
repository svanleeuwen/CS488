#ifndef CS488_INTERVAL_HPP
#define CS488_INTERVAL_HPP

#include <stdlib.h>
#include <math.h>

#include "algebra.hpp"

class Interval {
public: 
    Interval();
    Interval(double low, double high);
    Interval(double val);
    Interval(const Interval& other);
   
    ~Interval();

    Interval& operator=(const Interval& other);

    double& operator[](size_t idx) 
    {
        return v_[ idx ];
    }
    double operator[](size_t idx) const 
    {
        return v_[ idx ];
    }

    Interval reciprocal();
   
    static bool less(const Interval& a, const Interval& b);

    static Interval set_intersection(const Interval& a, const Interval& b);
    static Interval set_union(const Interval& a, const Interval& b);

    void extend(double val);

    bool isEmpty() { return m_empty; }

private:
    void copy(const Interval& other);

    double v_[2];
    bool m_empty;
};

inline Interval operator+(const Interval& a, const Interval& b) {
    return Interval(a[0] + b[0], a[1] + b[1]);
}

inline Interval operator-(const Interval& a, const Interval& b) {
    return Interval(a[0] - b[0], a[1] - b[1]);
}

inline Interval operator-(const Interval& a) {
    return Interval(-a[1], -a[0]);
}

inline Interval operator+(const Interval& a, double val){
    return Interval(a[0] + val, a[1] + val);
}

inline Interval operator-(const Interval& a, double val){
    return Interval(a[0] - val, a[1] - val);
}

Interval operator*(const Interval& a, const Interval& b);

Interval operator*(const Interval& a, double val);

class IVector3D {
public:
    IVector3D();
    IVector3D(const Interval& ix,const Interval& iy,const Interval& iz);
    IVector3D(const Vector3D& vec);
    IVector3D(const Point3D& point);

    IVector3D(const IVector3D& other);
    IVector3D& operator=(const IVector3D& other);

    Interval& operator[](size_t idx) 
    {
        return v_[ idx ];
    }
    Interval operator[](size_t idx) const 
    {
        return v_[ idx ];
    }

    Interval dot(const IVector3D& other) const
    {
        return v_[0]*other.v_[0] + v_[1]*other.v_[1] + v_[2]*other.v_[2];
    }

    void extend(const Vector3D& vec);
    void extend(const Point3D& vec);

    IVector3D reciprocal();
    Interval intersectMembers();

private:
    void copy(const IVector3D& other);
    Interval v_[3];

    bool m_empty;
};

inline IVector3D operator *(const Interval& s, const IVector3D& v)
{
  return IVector3D(s*v[0], s*v[1], s*v[2]);
}

inline IVector3D operator *(const IVector3D& v, const Interval& s)
{
  return IVector3D(s*v[0], s*v[1], s*v[2]);
}

inline IVector3D operator +(const IVector3D& a, const IVector3D& b)
{
  return IVector3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline IVector3D operator *(const IVector3D& a, const IVector3D& b)
{
  return IVector3D(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}

inline IVector3D operator -(const IVector3D& a, const IVector3D& b)
{
  return IVector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline IVector3D operator -(const IVector3D& a)
{
  return IVector3D(-a[0], -a[1], -a[2]);
}

#endif
