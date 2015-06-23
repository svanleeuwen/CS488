#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "ray.hpp"
#include "light.hpp"
#include "intersection.hpp"

class Material {
public:
  virtual ~Material();
  virtual Colour getColour(const Vector3D& in, const Vector3D& out, const Intersection& isect, const Light& light) = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual Colour getColour(const Vector3D& in, const Vector3D& out, const Intersection& isect, const Light& light);

  Colour getKD() const { return m_kd; }

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
