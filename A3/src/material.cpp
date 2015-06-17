#include "material.hpp"
#include "Viewer.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl(Viewer* view) const
{
    view->set_colour(QColor(m_kd.R()*255, m_kd.G()*255, m_kd.B()*255));
}
