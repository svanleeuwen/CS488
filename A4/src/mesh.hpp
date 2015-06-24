#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"
#include "intersection.hpp"
#include "scene.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
    Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

    virtual bool getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object);

    typedef std::vector<int> Face;
  
private:
    bool getPolyIntersection(const Face& poly, Intersection* isect);
    bool getPlaneIntersection(const Face& poly, const Ray& ray, Intersection* isect, GeometryNode* object);
    
    Vector3D getPolyNormal(const Face& poly);
    Point3D getInnerPoint(const Face& poly);

    std::vector<Point3D> m_verts;
    std::vector<Face> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
