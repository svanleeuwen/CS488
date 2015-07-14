#include "tracer.hpp"
#include "material.hpp"

#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;
using std::vector;

#define MAX_DEPTH 5
#define REFLECTION_ATTENUATION 0.5

static Primitive** unpackPrimitives(vector<Primitive*>* primitives) {
    Primitive** primArray = new Primitive* [primitives->size()];
    for(uint i = 0; i < primitives->size(); i++) {
        primArray[i] = primitives->at(i);
    }

    return primArray;
}

Tracer::Tracer(std::vector<Primitive*>* primitives, const Colour& ambient, const std::list<Light*>* lights) :
    m_primitives(primitives), m_ambient(ambient), m_lights(lights) 
{
#ifdef BIH
    Primitive** primArray = unpackPrimitives(primitives);
    m_bih = new BIHTree(primArray, primitives->size());
#endif
}

#ifndef BIH
bool Tracer::getIntersection(const Ray& ray, Intersection* isect) {
    Ray testRay = ray;

    Intersection* best = (isect == NULL) ? NULL : new Intersection();
    bool hitAny = false;

    for(auto it = m_primitives->begin(); it != m_primitives->end(); it++) {
        bool hit = (*it)->getIntersection(testRay, best);

        if(isect != NULL && hit) {
            hitAny = true;
            testRay = Ray(testRay.getOrigin(), best->getPoint());
        
        } else if(hit) {
            return true;
        }
    }

    if(isect != NULL) {
        if(hitAny) {
            *isect = *best;
        }
        delete best;
    }

    return hitAny;
}
#else
bool Tracer::getIntersection(const Ray& ray, Intersection* isect) {
    return m_bih->getIntersection(ray, isect);
}
#endif

Colour Tracer::castShadowRays(const Ray& ray, Intersection* isect) {
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

Colour Tracer::castReflectionRay(const Ray& ray, Intersection* isect, int depth) {
    if(depth > MAX_DEPTH) {
        return Colour(0.0, 0.0, 0.0);
    }

    Vector3D dir = -ray.getDirection();
    Vector3D norm = isect->getNormal();

    if(norm.dot(dir) < 0) {
        norm = -norm;
    }

    Vector3D refl = 2 * norm.dot(dir) * norm - dir;

    PhongMaterial* material = isect->getMaterial();
    Colour ks = material->getKS();

    Ray reflected = Ray(isect->getPoint(), refl);
    Colour colour(0.0, 0.0, 0.0);
    
    traceRay(reflected, colour, depth);
    return REFLECTION_ATTENUATION * ks * colour;
}

// I got the equation for refracted from the Wikipedia entry for Snell's law
Colour Tracer::castRefractionRay(const Ray& ray, Intersection* isect, int depth) {
    if(depth > MAX_DEPTH) {
        return Colour(0.0, 0.0, 0.0);
    }

    Vector3D in = ray.getDirection();
    Vector3D norm = isect->getNormal();

    double cos_theta1 = -in.dot(norm);
    bool incoming = cos_theta1 >= 0.0;

    if(!incoming) {
        norm = -norm;
        cos_theta1 = -cos_theta1;
    }

    PhongMaterial* material = isect->getMaterial();
    double materialMedium = material->getMedium();

    double mediumRatio;

    if(incoming) {
        mediumRatio = 1.0 / materialMedium;
    } else {
        mediumRatio = materialMedium;
    }

    double cos_theta2 = sqrt(1 - (mediumRatio * mediumRatio) * (1 - (cos_theta1 * cos_theta1)));
    Vector3D refracted = (mediumRatio * in) + ((mediumRatio * cos_theta1) - cos_theta2) * norm;

    Ray refractedRay(isect->getPoint(), refracted);
    Colour colour(0.0, 0.0, 0.0);

    traceRay(refractedRay, colour, depth);
    return colour;
}

bool Tracer::traceRay(Ray& ray, Colour& colour, int depth) {
    Intersection* isect = new Intersection();
    bool hit = getIntersection(ray, isect);

    if(!hit) {
        delete isect;
        return false;
    }

    PhongMaterial* material = isect->getMaterial();
    double transmitRatio = material->getTransmitRatio();
    double reflectRatio = 1.0 - transmitRatio;

    if(reflectRatio > 1.0e-10) {    
        if(material->isDiffuse()) {
            colour = reflectRatio * material->getKD() * m_ambient;
        }

        colour += reflectRatio * castShadowRays(ray, isect);
        
        if(material->isSpecular()) {
            colour += reflectRatio * castReflectionRay(ray, isect, depth + 1);
        }
    }

    if(transmitRatio > 1.0e-10) {
        colour += transmitRatio * castRefractionRay(ray, isect, depth + 1);
    }

    delete isect;
    return true;
}
