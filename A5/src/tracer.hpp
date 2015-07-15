#ifndef TRACER_HPP
#define TRACER_HPP

#include "camera.hpp"
#include "algebra.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "primitive.hpp"
#include "bih.hpp"

#include <list>
#include <vector>

class Tracer {
public:
    Tracer(std::vector<Primitive*>* primitives, const Colour& ambient, const std::list<Light*>* lights);

#ifdef BIH
    void tracePacket(Packet& packet, std::vector<Colour>& colours, std::vector<bool>& v_hit, int depth = 0);
#endif
    bool traceRay(Ray& ray, Colour& colour, int depth = 0);

private:
    Colour castShadowRays(const Ray& ray, Intersection* isect);
    Colour castReflectionRay(const Ray& ray, Intersection* isect, int depth);
    Colour castRefractionRay(const Ray& ray, Intersection* isect, int depth);

    bool getIntersection(const Ray& ray, Intersection* isect);

    std::vector<Primitive*>* m_primitives;
#ifdef BIH
    BIHTree* m_bih;    
#endif

    const Camera* m_cam;
    Colour m_ambient;
    const std::list<Light*>* m_lights;
};

#endif
