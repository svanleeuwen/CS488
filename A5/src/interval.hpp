#ifndef CS488_INTERVAL_HPP
#define CS488_INTERVAL_HPP

#include <stdlib.h>
#include <math.h>

enum Tool {
    True = 1,
    False = 0,
    Undecided = -1
};

class Interval {
public: 
    Interval(double low, double high);
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

    Interval reciprocal(const Interval& a);

    static Tool less(const Interval& a, const Interval& b);

    static Interval set_intersection(const Interval& a, const Interval& b);
    static Interval set_union(const Interval& a, const Interval& b);

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

#endif
