#include "light.hpp"
#include <iostream>

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

Colour Light::getIntensity(Point3D& point) const {
    double r = (point - position).length();
    double ratio;
    
    if(falloff[1] > 0 || falloff[2] > 0) {
        ratio = 1 / (falloff[0] + falloff[1]*r + falloff[2]*r*r); 
    } else {
        ratio = 1 / r*r;
    }

    return ratio * colour; 
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << l.colour << ", " << l.position << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
