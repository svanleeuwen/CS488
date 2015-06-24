#include "tracer.hpp"
#include "material.hpp"
#include <iostream>

using std::cout;
using std::endl;

Colour Tracer::castShadowRays(Ray& ray, Intersection& isect) {
    Colour colour = Colour(0.0, 0.0, 0.0);
    Material* material = isect.getObject()->get_material();

    for(auto it = m_lights->begin(); it != m_lights->end(); it++) {
        Point3D origin = isect.getPoint();
        Ray shadowRay = Ray(origin, (*it)->position);

        if(!m_scene->exists_intersection(shadowRay)) {
            colour += material->getColour(shadowRay.getDirection(), -ray.getDirection(), isect, *(*it));
        }
    } 

    return colour;
}

bool Tracer::traceRay(Ray& ray, Colour& colour) {
    Intersection isect;
    bool hit = m_scene->get_intersection(ray, &isect);

    if(!hit) {
        return false;
    }

    Material* material = isect.getObject()->get_material();
    colour = ((PhongMaterial*)material)->getKD() * m_ambient;

    colour += castShadowRays(ray, isect);
    return true;
}
