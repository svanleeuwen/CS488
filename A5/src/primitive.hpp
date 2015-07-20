#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "bbox.hpp"
#include "packet.hpp"
#include "map.hpp"
#include "material.hpp"

class Primitive {
public:
    Primitive() {}
    virtual ~Primitive();

    Primitive(const Primitive& other);
    Primitive& operator=(const Primitive& other);

    void setTransform(const Matrix4x4& trans, const Matrix4x4& inv);

    PhongMaterial* getMaterial() const { return m_material; }
    void setMaterial(PhongMaterial* material) { m_material = material; }
  
    Texture* getTexture() const { return m_texture; }
    void setTexture(Texture* texture) { m_texture = texture; }

    Bump* getBump() const { return m_bump; }
    void setBump(Bump* bump) { m_bump = bump; }

    virtual bool allMiss(const Packet& packet);
    void getIntersection(Packet& packet, int firstActive, std::vector<bool>& v_hit, std::vector<Intersection>* v_isect);   
    
    virtual Colour getColour(const Point3D& point);
    virtual Vector3D getOffset(const Point3D& point);
   
    virtual Primitive* clone() = 0;
    virtual bool getIntersection(const Ray& ray, Intersection* isect) = 0;

    virtual bool isMesh() { return false; }

    AABB* getWorldBBox() { return &m_worldBBox; }

protected:
    void setBBox(const Point3D& min, const Point3D& max);

    Matrix4x4 m_trans;
    Matrix4x4 m_inv;

    AABB m_modelBBox;
    AABB m_worldBBox;

    PhongMaterial* m_material;
    Texture* m_texture;
    Bump* m_bump;
};

class Sphere : public Primitive {
public:
    Sphere();
    virtual ~Sphere();

    Sphere(const Sphere& other);
    Sphere& operator=(const Sphere& other);
   
    virtual Sphere* clone() { return new Sphere(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);
};

class Cube : public Primitive {
public:
    Cube();
    virtual ~Cube();

    Cube(const Cube& other);
    Cube& operator=(const Cube& other);

    virtual Cube* clone() { return new Cube(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);

    virtual Colour getColour(const Point3D& point);
    virtual Vector3D getOffset(const Point3D& point);
};

class NonhierSphere : public Primitive {
public:
    NonhierSphere(const Point3D& pos, double radius);
    virtual ~NonhierSphere();

    NonhierSphere(const NonhierSphere& other);
    NonhierSphere& operator=(const NonhierSphere& other);

    virtual NonhierSphere* clone() { return new NonhierSphere(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);

private:
    Point3D m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
public:
    NonhierBox(const Point3D& pos, double size);
    virtual ~NonhierBox();

    NonhierBox(const NonhierBox& other);
    NonhierBox& operator=(const NonhierBox& other);

    virtual NonhierBox* clone() { return new NonhierBox(*this); }
    virtual bool getIntersection(const Ray& ray, Intersection* isect);

private:
    Point3D m_pos;
    double m_size;
};

#endif
