#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Viewer;

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl(Viewer* view, bool picking) const = 0;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual void walk_gl(Viewer* view, bool picking) const;
};

#endif
