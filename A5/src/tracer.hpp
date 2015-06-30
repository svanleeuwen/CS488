#ifndef TRACER_HPP
#define TRACER_HPP

#include "camera.hpp"
#include "algebra.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "primitive.hpp"

#include <list>
#include <vector>

class Tracer {
public:
#ifndef BIH
    Tracer(std::vector<Primitive*>* primitives, const Camera& cam, const Colour& ambient, const std::list<Light*>& lights) :
        m_primitives(primitives), m_cam(&cam), m_ambient(ambient), m_lights(&lights) {}
#endif

    bool traceRay(Ray& ray, Colour& colour);

private:

    Colour castShadowRays(Ray& ray, Intersection* isect);

#ifndef BIH
    bool getIntersection(Ray& ray, Intersection* isect);

    std::vector<Primitive*>* m_primitives;
#endif
    const Camera* m_cam;
    Colour m_ambient;
    const std::list<Light*>* m_lights;
};

#endif
