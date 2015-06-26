#ifndef TRACER_HPP
#define TRACER_HPP

#include "camera.hpp"
#include "algebra.hpp"
#include <list>
#include "light.hpp"
#include "ray.hpp"
#include "scene.hpp"

class Tracer {
public:
    Tracer(SceneNode* scene, const Camera& cam, const Colour& ambient, const std::list<Light*>& lights) :
        m_scene(scene), m_cam(&cam), m_ambient(ambient), m_lights(&lights) {}

    bool traceRay(Ray& ray, Colour& colour);

private:

    Colour castShadowRays(Ray& ray, Intersection& isect);

    SceneNode* m_scene;
    const Camera* m_cam;
    Colour m_ambient;
    const std::list<Light*>* m_lights;
};

#endif
