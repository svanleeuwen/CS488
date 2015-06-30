#include "mesh.hpp"
#include <iostream>

using std::cout;
using std::endl;

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
    Point3D a_min = verts.at(0);
    Point3D a_max = verts.at(0);

    for(auto it = verts.begin(); it != verts.end(); it++) {
        for(int i = 0; i < 3; i++) {
            if((*it)[i] < a_min[i]) {
                a_min[i] = (*it)[i];
            }
            if((*it)[i] > a_max[i]) {
                a_max[i] = (*it)[i];
            }
        }   
    }

    setBBox(a_min, a_max);
}

Mesh::Mesh(const Mesh& other) : Primitive(other)
{
    m_verts = other.m_verts;
    m_faces = other.m_faces;
}

Mesh& Mesh::operator=(const Mesh& other) {
    Primitive::operator=(other);

    m_verts = other.m_verts;
    m_faces = other.m_faces;
    
    return *this;
}

int getMaxIndex(Vector3D& vec) {
    double max = fabs(vec[0]);
    int index = 0;

    for(int i = 1; i < 3; i++) {
        double val = fabs(vec[i]);
        if( val > max) {
            max = val;
            index = i;
        }
    }

    return index;
}

bool Mesh::getIntersection(const Ray& ray, Intersection* isect) {
    Ray modelRay = ray.getTransform(m_inv);
    
    if(!m_modelBBox.intersect(modelRay)) {
        return false;
    }
    
    Intersection* best = NULL;

    for(auto it = m_faces.begin(); it != m_faces.end(); it++) {
        Intersection* next = new Intersection();
        bool hit = getPlaneIntersection(*it, modelRay, next);

        if(hit) {
            hit = getPolyIntersection(*it, next);
        }

        if(hit && isect == NULL && modelRay.checkParam(next->getParam())) {
            delete next;
            return true;
        
        } else if(hit) {
            if(modelRay.checkParam(next->getParam())) {
                if(best == NULL) {
                    best = next;
                    next = NULL;
    
                } else {
                    if(next->getParam() < best->getParam()) {
                        delete best;
                        
                        best = next;
                        next = NULL;
                    }
                }
            }
        }

        if(next != NULL) {
            delete next;
        }
    }

    if(isect != NULL && best != NULL) {
        Point3D point = m_trans * best->getPoint();
        Vector3D normal = m_inv.transpose() * best->getNormal();

        *isect = Intersection(point, best->getParam(), m_material, normal);
        delete best;
        return true;
    }

    return false;
}

Vector3D getLineNormal(const Point3D& p1, const Point3D& p2, const Point3D& innerPoint, const Vector3D& polyNormal) {
    Vector3D line = p2 - p1;
    Vector3D normal = line.cross(polyNormal);

    if(normal.dot(innerPoint - p1) < 0) {
        normal = -normal;
    }

    return normal;
}

bool Mesh::getPolyIntersection(const Face& poly, Intersection* isect) {
    Point3D point = isect->getPoint();
    int n = poly.size();

    for(int i = 0; i < n; i++) {
        int a = poly.at(i);
        int b = poly.at((i+1) % n);

        Point3D p1 = m_verts.at(a);
        Point3D p2 = m_verts.at(b);

        Vector3D normal = getLineNormal(p1, p2, getInnerPoint(poly), isect->getNormal());

        bool inside = (point - p1).dot(normal) > 0;
        if(!inside) {
            return false;
        }
    }

    return true;
}

bool Mesh::getPlaneIntersection(const Face& poly, const Ray& ray, Intersection* isect) {
    Vector3D normal = getPolyNormal(poly);
    Point3D p = m_verts.at(poly.at(0));

    Point3D o = ray.getOrigin();
    Vector3D d = ray.getDirection();

    if(abs(normal.dot(d)) < 1.0e-10) {
        return false;
    }

    double t = (normal.dot(Vector3D(p)) - normal.dot(Vector3D(o))) / (normal.dot(d));
    *isect = Intersection(ray(t), t, m_material, normal);

    return true;
}

Vector3D Mesh::getPolyNormal(const Face& poly) {
    Point3D p1 = m_verts.at(poly.at(0));
    Point3D p2 = m_verts.at(poly.at(1));
    Point3D p3 = m_verts.at(poly.at(2));

    return (p2 - p1).cross(p3 - p1);
}

Point3D Mesh::getInnerPoint(const Face& poly) {
    Point3D p1 = m_verts.at(poly.at(0));
    Point3D p2 = m_verts.at(poly.at(1));
    Point3D p3 = m_verts.at(poly.at(2));

    return 1.0/3.0 * (p1 + p2 + p3);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
