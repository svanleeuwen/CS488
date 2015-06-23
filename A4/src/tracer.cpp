#include "tracer.hpp"
#include "material.hpp"
#include <iostream>

using std::cout;
using std::endl;

Colour Tracer::castShadowRays(Ray& ray, Intersection& isect) {
    Colour colour = m_ambient;

    for(auto it = m_lights->begin(); it != m_lights->end(); it++) {
        Point3D origin = isect.getPoint();
        Ray shadowRay = Ray(origin, (*it)->position);

        if(!m_scene->exists_intersection(shadowRay)) {
            Material* material = isect.getObject()->get_material();
            colour += material->getColour(shadowRay.getDirection(), -ray.getDirection(), isect, *(*it));
        }
    }

    return colour;
}

Colour Tracer::traceRay(Ray& ray) {
    Intersection isect;
    bool hit = m_scene->get_intersection(ray, &isect);

    if(!hit) {
        // Return background colour for this pixel
        return Colour(0.0, 0.0, 0.3);
    }
    
    return castShadowRays(ray, isect);
}
