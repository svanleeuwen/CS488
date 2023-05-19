#include "interval.hpp"
#include <limits>
#include <iostream>

using std::min;
using std::max;
using std::cout;
using std::endl;

// ************************* Interval ********************************

Interval::Interval() : m_empty(true)
{}

Interval::Interval(double low, double high)
{
    v_[0] = low;
    v_[1] = high;

    m_empty = low > high;
}

Interval::Interval(double val) : Interval(val, val) {}

Interval::~Interval() {}

void Interval::copy(const Interval& other) {
    v_[0] = other[0];
    v_[1] = other[1];
    m_empty = other.m_empty;
}

Interval::Interval(const Interval& other) {
    copy(other);    
}

Interval& Interval::operator=(const Interval& other) {
    if(this != &other) {
        copy(other);
    }
    return *this;
}

Interval Interval::reciprocal() {
    if(v_[0] < 1.0e-10 || v_[1] < 1.0e-10) {
        return Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    } else {
        return Interval(1.0/v_[1], 1.0/v_[0]);
    }
}

bool Interval::less(const Interval& a, const Interval& b) {
    // Use a ternary enum here if need comes up
    return a[1] < b[0];
}

Interval Interval::set_intersection(const Interval& a, const Interval& b) {
    return Interval(max(a[0], b[0]), min(a[1], b[1]));
}

Interval Interval::set_union(const Interval& a, const Interval& b) {
    return Interval(min(a[0], b[0]), max(a[1], b[1]));
}

void Interval::extend(double val) {
    if(m_empty) {
        m_empty = false;
        v_[0] = val;
        v_[1] = val;
        return;
    }

    v_[0] = min(v_[0], val);
    v_[1] = max(v_[1], val);
}

Interval operator*(const Interval& a, const Interval& b) {
    double min1 = min(a[0]*b[0], a[0]*b[1]);
    double min2 = min(a[1]*b[0], a[1]*b[1]);

    double max1 = max(a[0]*b[0], a[0]*b[1]);
    double max2 = max(a[1]*b[0], a[1]*b[1]);

    return Interval(min(min1, min2), max(max1, max2));
}

Interval operator*(const Interval& a, double val) {
    double low = a[0] * val;
    double high = a[1] * val;

    if(low <= high) {
        return Interval(low, high);
    } else {
        return Interval(high, low);
    }
}

// ************************* IVector3D ********************************

IVector3D::IVector3D() {
    m_empty = true;
}

IVector3D::IVector3D(const Interval& ix, const Interval& iy, const Interval& iz) {
    v_[0] = ix;
    v_[1] = iy;
    v_[2] = iz;

    m_empty = false;
}

IVector3D::IVector3D(const Vector3D& vec) {
    v_[0] = Interval(vec[0]);
    v_[1] = Interval(vec[1]);
    v_[2] = Interval(vec[2]);

    m_empty = false;
}

IVector3D::IVector3D(const Point3D& point) {
    v_[0] = Interval(point[0]);
    v_[1] = Interval(point[1]);
    v_[2] = Interval(point[2]);

    m_empty = false;
}

void IVector3D::copy(const IVector3D& other) {
    v_[0] = other[0];
    v_[1] = other[1];
    v_[2] = other[2];

    m_empty = other.m_empty;
}

IVector3D::IVector3D(const IVector3D& other) {
    copy(other);
}

IVector3D& IVector3D::operator=(const IVector3D& other) {
    if(this != &other) {
        copy(other);
    } 
    return *this;
}

void IVector3D::extend(const Vector3D& vec) {
    if(m_empty) {
        IVector3D(vec);
        m_empty = false;
        return;
    }

    for(int i = 0; i < 3; i++) {
        v_[i].extend(vec[i]);
    }
}

void IVector3D::extend(const Point3D& point) {
    if(m_empty) {
        IVector3D(point);
        m_empty = false;
        return;
    }

    for(int i = 0; i < 3; i++) {
        v_[i].extend(point[i]);
    }
}

IVector3D IVector3D::reciprocal() {
    return IVector3D(v_[0].reciprocal(), v_[1].reciprocal(), v_[2].reciprocal());
}

Interval IVector3D::intersectMembers() {
    return Interval::set_intersection(Interval::set_intersection(v_[0], v_[1]), v_[2]);
}
