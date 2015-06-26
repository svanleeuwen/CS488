#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
    a_min = verts.at(0);
    a_max = verts.at(0);

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
}

bool Mesh::intersectsBoundingBox(const Ray& ray) {
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    Vector3D p_min = a_min - ray.getOrigin();
    Vector3D p_max = a_max - ray.getOrigin();

    Vector3D d = ray.getDirection();
    bool finite_ray = ray.hasEndpoint();

    for(int i = 0; i < 3; i++) {
        if(fabs(d[i]) > 1.0e-15) {
        double t1 = p_max[i] / d[i];
        double t2 = p_min[i] / d[i];

        if(t1 < t2) {
            if(t1 > t_min) {
                t_min = t1;
            }
            if(t2 < t_max) {
                t_max = t2;
            }
        } else {
            if(t2 > t_min) {
                t_min = t2;
            }
            if(t1 < t_max) {
                t_max = t1;
            }
        }

        if(t_min > t_max || t_max < ray.getEpsilon()) {
            return false;
        }

        } else if(-p_max[i] > 0 || -p_min[i] < 0) {
            return false;
        }

        if(finite_ray && t_min > 1) {
            return false;
        }
    }

    return true;
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

bool Mesh::getIntersection(const Ray& ray, Intersection* isect, GeometryNode* object) {
    if(!intersectsBoundingBox(ray)) {
        return false;
    }
    
    bool hitAny = false;

    for(auto it = m_faces.begin(); it != m_faces.end(); it++) {
        Intersection t_isect;
        bool hit = getPlaneIntersection(*it, ray, &t_isect, object);

        if(hit) {
            hit = getPolyIntersection(*it, &t_isect);
        }

        if(hit) {
            if(ray.checkParam(t_isect.getParam())) {
                if(!hitAny) {
                    hitAny = true;
                    *isect = t_isect;
                } else {
                    if(t_isect.getParam() < isect->getParam()) {
                        *isect = t_isect;
                    }
                }
            }

        }
    }

    return hitAny;
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

bool Mesh::getPlaneIntersection(const Face& poly, const Ray& ray, Intersection* isect, GeometryNode* object) {
    Vector3D normal = getPolyNormal(poly);
    Point3D p = m_verts.at(poly.at(0));

    Point3D o = ray.getOrigin();
    Vector3D d = ray.getDirection();

    if(abs(normal.dot(d)) < 1.0e-10) {
        return false;
    }

    double t = (normal.dot(Vector3D(p)) - normal.dot(Vector3D(o))) / (normal.dot(d));
    *isect = Intersection(ray(t), t, object, normal);

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