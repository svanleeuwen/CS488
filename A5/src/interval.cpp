#include "interval.hpp"
#include <limits>

Interval::Interval(double low, double high)
{
    v_[0] = low;
    v_[1] = high;

    m_empty = low > high;
}

void Interval::copy(const Interval& other) {
    v_[0] = other[0];
    v_[1] = other[1];
}

Interval::Interval(const Interval& other) {
    copy(other);    
}

Interval::~Interval() {}

Interval& Interval::operator=(const Interval& other) {
    if(this != &other) {
        copy(other);
    }

    return *this;
}

Interval Interval::reciprocal(const Interval& a) {
    if(a[0] < 1.0e-10 || a[1] < 1.0e-10) {
        return Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    } else {
        return Interval(1.0/a[1], 1.0/a[0]);
    }
}

Tool Interval::less(const Interval& a, const Interval& b) {
    if(a[1] < b[0]) {
        return Tool::True;
   
    } else if(a[0] >= b[1]) {
        return Tool::False;

    } else {
        return Tool::Undecided;
    }
}

Interval Interval::set_intersection(const Interval& a, const Interval& b) {
    return Interval(fmin(a[0], b[0]), fmax(a[1], b[1]));
}

Interval Interval::set_union(const Interval& a, const Interval& b) {
    return Interval(fmax(a[0], b[0]), fmin(a[1], b[1]));
}
Interval operator*(const Interval& a, const Interval& b) {
    double min1 = fmin(a[0]*b[0], a[0]*b[1]);
    double min2 = fmin(a[1]*b[0], a[1]*b[1]);

    double max1 = fmax(a[0]*b[0], a[0]*b[1]);
    double max2 = fmax(a[1]*b[0], a[1]*b[1]);

    return Interval(fmin(min1, min2), fmax(max1, max2));
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


