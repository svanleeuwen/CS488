#include "primitive.hpp"
#include "Viewer.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Sphere::walk_gl(Viewer* view, bool picking) const
{
    view->drawSphere(picking);
}
