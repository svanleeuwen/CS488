#include "tracer.hpp"
#include "material.hpp"

#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;
using std::cerr;
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
    if(BIH) { 
        Primitive** primArray = unpackPrimitives(primitives);
        m_bih = new BIHTree(primArray, primitives->size());
    } else {
        m_bih = NULL;
    }

}

void Tracer::updatePrimitives(vector<Primitive*>* primitives) {
    m_primitives = primitives;
    
    if(BIH) {
        if(m_bih != NULL) {
            delete m_bih;
        }

        Primitive** primArray = unpackPrimitives(primitives);
        m_bih = new BIHTree(primArray, primitives->size());
    }
}

bool Tracer::getIntersection(const Ray& ray, Intersection* isect) {
    if(BIH) {
        return m_bih->getIntersection(ray, isect);
    }

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

Colour Tracer::castShadowRays(const Ray& ray, Intersection* isect) {
    Colour colour = Colour(0.0, 0.0, 0.0);
    Material* material = isect->getPrimitive()->getMaterial();

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

    Ray reflected = Ray(isect->getPoint(), refl);
    Colour colour(0.0, 0.0, 0.0);
    
    traceRay(reflected, colour, depth);

    Colour ks = isect->getSpecular();
    return REFLECTION_ATTENUATION * ks * colour;
}

// I got the equation for refracted from the Wikipedia entry for Snell's law
Ray getRefracted(const Ray& ray, Intersection* isect) {
    Vector3D in = ray.getDirection();
    Vector3D norm = isect->getNormal();

    double cos_theta1 = -in.dot(norm);
    bool incoming = cos_theta1 >= 0.0;

    if(!incoming) {
        norm = -norm;
        cos_theta1 = -cos_theta1;
    }

    PhongMaterial* material = isect->getPrimitive()->getMaterial();
    double materialMedium = material->getMedium();

    double mediumRatio;

    if(incoming) {
        mediumRatio = 1.0 / materialMedium;
    } else {
        mediumRatio = materialMedium;
    }

    double cos_theta2 = sqrt(1 - (mediumRatio * mediumRatio) * (1 - (cos_theta1 * cos_theta1)));
    Vector3D refracted = (mediumRatio * in) + ((mediumRatio * cos_theta1) - cos_theta2) * norm;

    return Ray(isect->getPoint(), refracted);
}

Colour Tracer::castRefractionRay(const Ray& ray, Intersection* isect, int depth) {
    if(depth > MAX_DEPTH) {
        return Colour(0.0, 0.0, 0.0);
    }

    Colour colour(0.0, 0.0, 0.0);

    Ray refractedRay = getRefracted(ray, isect);
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

    PhongMaterial* material = isect->getPrimitive()->getMaterial();
    double transmitRatio = material->getTransmitRatio();
    double reflectRatio = 1.0 - transmitRatio;

    if(reflectRatio > 1.0e-10) {    
        if(material->isDiffuse()) {
            colour = reflectRatio * isect->getDiffuse() * m_ambient;
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

void Tracer::castShadowRays(const vector<Ray*>* rays, ColourVector* colours, 
        const vector<bool>& v_hit, vector<Intersection>* v_isect)
{
    int n = rays->size();
    vector<bool>* l_hits = new vector<bool>(n);

    for(auto it_light = m_lights->begin(); it_light != m_lights->end(); ++it_light) {
        vector<Ray*>* shadowRays = new vector<Ray*>(n);
        int j = 0;

        for(int i = 0; i < n; ++i) {
            if(v_hit.at(i)) {
                shadowRays->at(i) = new Ray((*it_light)->position, v_isect->at(i).getPoint());
            } else {
                shadowRays->at(i) = NULL;
                j++;
            }
        }

        if(j != n) {
            Packet packet;
            packet.setRays(shadowRays);

            m_bih->getIntersection(packet, *l_hits, NULL);
            
            for(int i = 0; i < n; ++i) {
                if(v_hit.at(i) && !l_hits->at(i)) {
                    Intersection* isect = &v_isect->at(i);
                    Material* material = isect->getPrimitive()->getMaterial();

                    Colour shadowColour = 
                        material->getColour(
                            -shadowRays->at(i)->getDirection(),
                            -rays->at(i)->getDirection(), 
                            isect, 
                            *(*it_light)
                        );

                    colours->at(i) += shadowColour;
                }
            }
        } else {
            delete shadowRays;
        }
    }

    delete l_hits;
}

void Tracer::castReflectionRays(const vector<Ray*>* rays, ColourVector* colours, 
        const vector<bool>& v_hit, vector<Intersection>* v_isect, int depth) 
{
    if(depth > MAX_DEPTH) {
        return;
    }

    int n = rays->size();
    vector<bool>* l_hits = new vector<bool>(n);

    vector<Ray*>* reflectionRays = new vector<Ray*>(n);
    int j = 0;

    for(int i = 0; i < n; ++i) {
        if(v_hit.at(i)) {
            Intersection* isect = &v_isect->at(i);
            PhongMaterial* material = v_isect->at(i).getPrimitive()->getMaterial();

            if(material->isSpecular() && 1 - material->getTransmitRatio() > 1.0e-10) {
                Vector3D dir = -rays->at(i)->getDirection();
                Vector3D norm = isect->getNormal();

                if(norm.dot(dir) < 0) {
                    norm = -norm;
                }

                Vector3D refl = 2 * norm.dot(dir) * norm - dir;

                reflectionRays->at(i) = new Ray(isect->getPoint(), refl);
                continue;
            }
        }
        
        reflectionRays->at(i) = NULL;
        j++;
    }

    if(j > (n/2.0)) {
        for(int i = 0; i < n; i++) {
            Ray* ray = reflectionRays->at(i);

            if(ray != NULL) {
                bool hit = traceRay(*ray, colours->at(i), depth+1);

                if(hit) { 
                    Colour ks = v_isect->at(i).getSpecular();
                    colours->at(i) *= REFLECTION_ATTENUATION * ks;
                }

                delete ray;
            }
        }

        delete reflectionRays;

    } else if(j < n) {
        Packet packet;
        packet.setRays(reflectionRays);

        tracePacket(packet, colours, *l_hits, depth + 1);

        for(int i = 0; i < n; ++i) {
            if(v_hit.at(i) && l_hits->at(i)) {
                Colour ks = v_isect->at(i).getSpecular();
                colours->at(i) *= REFLECTION_ATTENUATION * ks;
            }
        }

    } else {
        delete reflectionRays;
    }

    delete l_hits;
}

void Tracer::castRefractionRays(const vector<Ray*>* rays, ColourVector* colours, 
        const vector<bool>& v_hit, vector<Intersection>* v_isect, int depth)
{
    if(depth > MAX_DEPTH) {
        return;
    }

    int n = rays->size();
    vector<bool>* l_hits = new vector<bool>(n);

    vector<Ray*>* refractionRays = new vector<Ray*>(n);
    int j = 0;

    for(int i = 0; i < n; ++i) {
        if(v_hit.at(i)) {
            Intersection* isect = &v_isect->at(i);
            PhongMaterial * material = isect->getPrimitive()->getMaterial();

            if(material->getTransmitRatio() > 1.0e-10) {
                refractionRays->at(i) = new Ray(getRefracted(*rays->at(i), isect));
                continue;
            }
        }

        refractionRays->at(i) = NULL;
        j++;
    }

/*    if(false) {//j > (n/2.0)) {
        for(int i = 0; i < n; i++) {
            Ray* ray = refractionRays->at(i);
            if(ray != NULL) {
                traceRay(*ray, colours->at(i), depth+1);
                delete ray;
            }
        }
        delete refractionRays;

    } else */
    if(j < n) {
        Packet packet;
        packet.setRays(refractionRays);

        tracePacket(packet, colours, *l_hits, depth + 1);
    } else {
        delete refractionRays;
    }

    delete l_hits;
}

void Tracer::tracePacket(Packet& packet, ColourVector* colours, vector<bool>& v_hit, int depth) {
    vector<Ray*>* rays = packet.getRays();
    int n = rays->size();

    vector<Intersection>* v_isect = new vector<Intersection>(n);
    m_bih->getIntersection(packet, v_hit, v_isect); 

    ColourVector* shadowColours = new ColourVector(n);
    castShadowRays(rays, shadowColours, v_hit, v_isect);

#ifndef NO_SECONDARY
    ColourVector* reflectColours = new ColourVector(n);
    castReflectionRays(rays, reflectColours, v_hit, v_isect, depth + 1);

    ColourVector* refractColours = new ColourVector(n);
    castRefractionRays(rays, refractColours, v_hit, v_isect, depth + 1);
#endif

    for(int i = 0; i < n; i++) {
        if(v_hit.at(i)) {
            Intersection* isect = &v_isect->at(i);

            PhongMaterial* material = isect->getPrimitive()->getMaterial();
            double transmitRatio = material->getTransmitRatio();
            double reflectRatio = 1.0 - transmitRatio;

            if(reflectRatio > 1.0e-10) {    
                if(material->isDiffuse()) {
                    colours->at(i) = reflectRatio * isect->getDiffuse() * m_ambient;
                }

                colours->at(i) += reflectRatio * shadowColours->at(i);
                
                if(material->isSpecular()) {
#ifdef NO_SECONDARY
                    colours->at(i) += reflectRatio * castReflectionRay(*rays->at(i), isect, depth + 1);
#else
                    colours->at(i) += reflectRatio * reflectColours->at(i);
#endif
                }
            }

            if(transmitRatio > 1.0e-10) {
#ifdef NO_SECONDARY
                colours->at(i) += transmitRatio * castRefractionRay(*rays->at(i), isect, depth + 1);
#else
                colours->at(i) += transmitRatio * refractColours->at(i);
#endif
            }
        }
    }

    delete v_isect;
    delete shadowColours;
    delete reflectColours;
    delete refractColours;
}
