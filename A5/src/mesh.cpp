#include "mesh.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

// ************************ static functions *****************************

static Vector3D getLineNormal(const Point3D& p1, const Point3D& p2, const Point3D& innerPoint, const Vector3D& polyNormal) {
    Vector3D line = p2 - p1;
    Vector3D normal = line.cross(polyNormal);

    if(normal.dot(innerPoint - p1) < 0) {
        normal = -normal;
    }

    return normal;
}

// ****************************** Mesh ***********************************

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
    Point3D a_min = verts.at(0);
    Point3D a_max = verts.at(0);

    for(auto it = verts.begin() + 1; it != verts.end(); it++) {
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
        Vector3D normal = transNorm(m_inv, best->getNormal());
        normal.normalize();

        *isect = Intersection(point, best->getParam(), this, normal);
        delete best;
        return true;
    }

    return false;
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

    if(fabs(normal.dot(d)) < 1.0e-10) {
        return false;
    }

    double t = (normal.dot(Vector3D(p)) - normal.dot(Vector3D(o))) / (normal.dot(d));
    *isect = Intersection(ray(t), t, this, normal);

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

void Mesh::addMeshPolygons(vector<Primitive*>* primitives) {
    if(m_faces.at(0).size() == 3) {
       for(auto it = m_faces.begin(); it != m_faces.end(); ++it) {
            Triangle* poly = new Triangle(m_verts, (*it), m_trans);

            poly->setMaterial(m_material);
            poly->setTexture(m_texture);
            poly->setBump(m_bump);

            primitives->push_back(poly);
        }
    } else if(m_faces.at(0).size() == 4) {
        for(auto it = m_faces.begin(); it != m_faces.end(); ++it) {
            Quad* poly = new Quad(m_verts, (*it), m_trans);

            poly->setMaterial(m_material);
            poly->setTexture(m_texture);
            poly->setBump(m_bump);

            primitives->push_back(poly);
        } 
    } else {
       for(auto it = m_faces.begin(); it != m_faces.end(); ++it) {
            Polygon* poly = new Polygon(m_verts, (*it), m_trans);

            poly->setMaterial(m_material);
            poly->setTexture(m_texture);
            poly->setBump(m_bump);

            primitives->push_back(poly);
        }
    }
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

// **************************** Polygon **********************************

Polygon::Polygon(const std::vector<Point3D>& verts, const std::vector<int>& indices,
        const Matrix4x4& trans) 
{
    Point3D point = trans * verts.at(indices.at(0));

    Point3D min = point;
    Point3D max = point;
    m_verts.push_back(point);

    for(auto it = indices.begin() + 1; it != indices.end(); ++it) {
        point = trans * verts.at(*it);

        min = Point3D::min(min, point);
        max = Point3D::max(max, point);

        m_verts.push_back(point);
    }

    setBBox(min, max); 

    Point3D p1 = m_verts.at(0);
    Point3D p2 = m_verts.at(1);
    Point3D p3 = m_verts.at(2);

    m_normal = (p2 - p1).cross(p3 - p1);
    m_normal.normalize();

    m_innerPoint = (1.0/3.0) * (p1 + p2 + p3);
}

Polygon::Polygon(const Polygon& other) : Primitive(other) 
{
    m_verts = other.m_verts;
    m_normal = other.m_normal;
    m_innerPoint = other.m_innerPoint;
}

Polygon& Polygon::operator=(const Polygon& other) {
    if(this != &other) {
        Primitive::operator=(other);

        m_verts = other.m_verts;
        m_normal = other.m_normal;
        m_innerPoint = other.m_innerPoint;       
    }

    return *this;
}

bool Polygon::getIntersection(const Ray& ray, Intersection* isect) {
    Intersection* t_isect = new Intersection();
    bool hit = getPlaneIntersection(ray, t_isect);

    if(!hit || !ray.checkParam(t_isect->getParam())) {
        delete t_isect;
        return false;
    }

    Point3D point = t_isect->getPoint();
    int n = m_verts.size();

    for(int i = 0; i < n; i++) {
        Point3D p1 = m_verts.at(i);
        Point3D p2 = m_verts.at((i+1) % n);

        Vector3D normal = getLineNormal(p1, p2, m_innerPoint, m_normal);

        bool inside = (point - p1).dot(normal) > 0;
        if(!inside) {
            delete t_isect;
            return false;
        }
    }

    if(isect != NULL) {
        *isect = *t_isect;
    }
    delete t_isect; 

    return true;
}

bool Polygon::getPlaneIntersection(const Ray& ray, Intersection* isect) {
    Point3D p = m_verts.at(0);

    Point3D o = ray.getOrigin();
    Vector3D d = ray.getDirection();

    if(fabs(m_normal.dot(d)) < 1.0e-10) {
        return false;
    }

    double t = (m_normal.dot(Vector3D(p)) - m_normal.dot(Vector3D(o))) / (m_normal.dot(d));
    *isect = Intersection(ray(t), t, this, m_normal);

    return true;
}

// ************************************ Triangle ****************************************

Triangle::Triangle(const std::vector<Point3D>& verts, const std::vector<int>& indices, const Matrix4x4& trans) :
    Polygon(verts, indices, trans)
{}
    
Triangle::Triangle(const Triangle& other) : Polygon(other)
{}

Triangle& Triangle::operator=(const Triangle& other) {
    Polygon::operator=(other);

    return *this;
}

// *********************************** Quad ***************************************

Quad::Quad(const std::vector<Point3D>& verts, const std::vector<int>& indices, const Matrix4x4& trans) :
    Polygon(verts, indices, trans)
{
    m_rotate[0][1] = m_normal[0];
    m_rotate[1][1] = m_normal[1];
    m_rotate[2][1] = m_normal[2];

    for(auto it = m_verts.begin(); it != m_verts.end(); ++it) {
        Point3D rotPoint = m_rotate * (*it);

        m_ix.extend(rotPoint[0]);
        m_iz.extend(rotPoint[2]);
    }
}
    
Quad::Quad(const Quad& other) : Polygon(other)
{}

Quad& Quad::operator=(const Quad& other) {
    Polygon::operator=(other);

    return *this;
}

Colour Quad::getColour(const Point3D& point) {
    Point3D rotPoint = m_rotate * point;

    double x = (rotPoint[0] - m_ix[0]) / (m_ix[1] - m_ix[0]);
    double z = (rotPoint[2] - m_iz[0]) / (m_iz[1] - m_iz[0]);

    return m_texture->getColour(Point2D(x, z));
}
