#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Viewer;

class Material {
public:
  virtual ~Material();
  virtual void apply_gl(Viewer* view) const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl(Viewer* view) const;

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
