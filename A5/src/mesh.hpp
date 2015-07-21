#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include "primitive.hpp"
#include "algebra.hpp"
#include "intersection.hpp"

#include <iosfwd>
#include <vector>

class Mesh : public Primitive {
public:
    Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

    Mesh(const Mesh& other);
    Mesh& operator=(const Mesh& other);

    virtual Mesh* clone() { return new Mesh(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);

    virtual bool isMesh() { return true; }
    void addMeshPolygons(std::vector<Primitive*>* primitives);

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

class Polygon : public Primitive {
public:
    Polygon(const std::vector<Point3D>& verts, const std::vector<int>& indices, const Matrix4x4& trans);
    
    Polygon(const Polygon& other);
    Polygon& operator=(const Polygon& other);

    virtual Polygon* clone() { return new Polygon(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);

protected:
    bool getPlaneIntersection(const Ray& ray, Intersection* isect);

    std::vector<Point3D> m_verts;
    
    Vector3D m_normal;
    Point3D m_innerPoint;
};

class Triangle : public Polygon {
public:
    Triangle(const std::vector<Point3D>& verts, const std::vector<int>& indices, const Matrix4x4& trans);
    
    Triangle(const Triangle& other);
    Triangle& operator=(const Triangle& other);
    
    virtual Triangle* clone() { return new Triangle(*this); }
};

class Quad : public Polygon {
public:
    Quad(const std::vector<Point3D>& verts, const std::vector<int>& indices, const Matrix4x4& trans);
    
    Quad(const Quad& other);
    Quad& operator=(const Quad& other);
    
    virtual Quad* clone() { return new Quad(*this); }
    virtual Colour getColour(const Point3D& point);
    virtual Vector3D getOffset(const Point3D& point);

private:
    Matrix4x4 m_rotate;
    Interval m_ix;
    Interval m_iz;
};

#endif
