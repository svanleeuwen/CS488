#include "map.hpp"

#include <QColor>
#include <iostream>

using std::cout;
using std::endl;

Map::Map(const char* filename) {
    m_img.load(QString(filename));
}

Texture::Texture(const char* filename) :
    Map(filename)
{}

static double clamp(double val, double min, double max) {
    if(val > max) {
        return max;
    }else if(val < min) {
        return min;
    } else {
        return val;
    }
}

// Uses Texture Mapping method described in class
Colour Texture::getColour(const Point2D& point) {
    double dx = (m_img.width() - 2) * clamp(point[0], 0.0, 1.0);
    double dy = (m_img.height() - 2) * clamp(point[1], 0.0, 1.0);

    int i = (int)dx;
    int j = (int)dy;

    double up = dx - i;
    double vp = dy - j;

    QColor c00 = QColor(m_img.pixel(i, j));
    QColor c01 = QColor(m_img.pixel(i, j+1));
    QColor c10 = QColor(m_img.pixel(i+1, j));
    QColor c11 = QColor(m_img.pixel(i+1, j+1));

    Colour d00 = Colour(c00.redF(), c00.greenF(), c00.blueF());
    Colour d01 = Colour(c01.redF(), c01.greenF(), c01.blueF());
    Colour d10 = Colour(c10.redF(), c10.greenF(), c10.blueF());
    Colour d11 = Colour(c11.redF(), c11.greenF(), c11.blueF());

    Colour interp = d00*(1-up)*(1-vp) + d01*(1-up)*vp + d10*(1-vp)*up + d11*up*vp;

    return interp;
}

Bump::Bump(const char* filename) :
    Map(filename)
{}

Point2D Bump::getOffset(const Point2D& point) {
    double dx = (m_img.width() - 5) * clamp(point[0], 0.0, 1.0) + 2;
    double dy = (m_img.height() - 5) * clamp(point[1], 0.0, 1.0) + 2;

    int i = (int)dx;
    int j = (int)dy;

    int h01 = qBlue(m_img.pixel(i-2, j));

    int h10 = qBlue(m_img.pixel(i, j-2));
    int h12 = qBlue(m_img.pixel(i, j+2));

    int h21 = qBlue(m_img.pixel(i+2, j));
    
    double du = (h21 - h01);
    double dv = (h12 - h10);

    return Point2D(du, dv);
}
