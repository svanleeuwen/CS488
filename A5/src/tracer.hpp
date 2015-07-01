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
#ifndef BIH
    Tracer(std::vector<Primitive*>* primitives, const Camera& cam, const Colour& ambient, const std::list<Light*>& lights) :
        m_primitives(primitives), m_cam(&cam), m_ambient(ambient), m_lights(&lights) {}
#else
    Tracer(BIHTree* bih, const Camera& cam, const Colour& ambient, const std::list<Light*>& lights) :
        m_bih(bih), m_cam(&cam), m_ambient(ambient), m_lights(&lights) {}
#endif

    bool traceRay(Ray& ray, Colour& colour);

private:
    Colour castShadowRays(Ray& ray, Intersection* isect);
    bool getIntersection(Ray& ray, Intersection* isect);

#ifndef BIH
    std::vector<Primitive*>* m_primitives;
#else
    BIHTree* m_bih;    
#endif

    const Camera* m_cam;
    Colour m_ambient;
    const std::list<Light*>* m_lights;
};

#endif
