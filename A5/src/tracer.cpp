#include "tracer.hpp"
#include "material.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

#ifndef BIH
bool Tracer::getIntersection(Ray& ray, Intersection* isect) {
    Intersection* best = NULL;

    for(auto it = m_primitives->begin(); it != m_primitives->end(); it++) {
        if(isect != NULL) {
            Intersection* next = new Intersection();
            bool hit = (*it)->getIntersection(ray, next);

            if(hit && best == NULL) {
                best = next;

            } else if(hit) {
                if(next->getParam() < best->getParam()) {
                    delete best;
                    best = next;
                
                } else {
                    delete next;
                }

            } else {
                delete next;
            }
        
        } else {
            bool hit = (*it)->getIntersection(ray, NULL);

            if(hit) {
                return true;
            }
        }
    }

    if(isect != NULL && best != NULL) {
        *isect = *best;
        delete best;

        return true;
    }

    return false;
}
#endif

Colour Tracer::castShadowRays(Ray& ray, Intersection* isect) {
    Colour colour = Colour(0.0, 0.0, 0.0);
    Material* material = isect->getMaterial();

    for(auto it = m_lights->begin(); it != m_lights->end(); it++) {
        Point3D origin = isect->getPoint();
        Ray shadowRay = Ray(origin, (*it)->position);

        if(!getIntersection(shadowRay, NULL)) {
            colour += material->getColour(shadowRay.getDirection(), -ray.getDirection(), isect, *(*it));
        }
    } 

    return colour;
}

bool Tracer::traceRay(Ray& ray, Colour& colour) {
    Intersection* isect = new Intersection();
    bool hit = getIntersection(ray, isect);

    if(!hit) {
        delete isect;
        return false;
    }

    Material* material = isect->getMaterial();
    colour = ((PhongMaterial*)material)->getKD() * m_ambient;

    colour += castShadowRays(ray, isect);
    delete isect;
    return true;
}
