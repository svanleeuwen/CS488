#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "ray.hpp"
#include "light.hpp"
#include "intersection.hpp"

class Material {
public:
  virtual ~Material();
  virtual Colour getColour(const Vector3D& in, const Vector3D& out, const Intersection* isect, const Light& light) = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double transmitRatio = 0.0, double medium = 1.0);
  virtual ~PhongMaterial();

  virtual Colour getColour(const Vector3D& in, const Vector3D& out, const Intersection* isect, const Light& light);

  Colour getKD() const { return m_kd; }
  Colour getKS() const { return m_ks; }

  double getTransmitRatio() const { return m_transmitRatio; }
  double getMedium() const { return m_medium; }

  double getShininess() const { return m_shininess; }

  inline bool isDiffuse() const { return m_kd.R() > 1.0e-10 || m_kd.G() > 1.0e-10 || m_kd.B() > 1.0e-10; }
  inline bool isSpecular() const { return m_ks.R() > 1.0e-10 || m_ks.G() > 1.0e-10 || m_ks.B() > 1.0e-10; }

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;

  double m_transmitRatio;
  double m_medium;
};

#endif
