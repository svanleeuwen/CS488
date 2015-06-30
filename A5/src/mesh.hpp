#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"
#include "intersection.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
    Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

    Mesh(const Mesh& other);
    Mesh& operator=(const Mesh& other);

    virtual Mesh* clone() { return new Mesh(*this); }

    virtual bool getIntersection(const Ray& ray, Intersection* isect);

    typedef std::vector<int> Face;
  
private:
    bool getPolyIntersection(const Face& poly, Intersection* isect);
    bool getPlaneIntersection(const Face& poly, const Ray& ray, Intersection* isect);
    
    Vector3D getPolyNormal(const Face& poly);
    Point3D getInnerPoint(const Face& poly);

    std::vector<Point3D> m_verts;
    std::vector<Face> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
