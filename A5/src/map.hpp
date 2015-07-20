#ifndef CS488_MAP_HPP
#define CS488_MAP_HPP

#include <QImage>
#include "algebra.hpp"

class Map {
public:
    Map(const char* filename);

    // pure virtual get value function

protected:
    QImage m_img;
};

class Texture : public Map {
public:
    Texture(const char* filename);

    Colour getColour(const Point2D& point);
};

class Bump : public Map {
public:
    Bump(const char* filename);
};

#endif
