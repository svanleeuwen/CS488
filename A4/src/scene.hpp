#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "intersection.hpp"

class SceneNode {
public:
    SceneNode(const std::string& name);
    virtual ~SceneNode();

    const Matrix4x4& get_transform() const { return m_trans ; }
    const Matrix4x4& get_inverse() const { return m_inv ; }

    void add_child(SceneNode* child)
    {
        m_children.push_back(child);
    }

    void remove_child(SceneNode* child)
    {
        m_children.remove(child);
    }

    virtual bool exists_intersection(const Ray& ray);
    virtual bool get_intersection(const Ray& ray, Intersection* isect);

    // Callbacks to be implemented.
    // These will be called from Lua.
    void rotate(char axis, double angle);
    void scale(const Vector3D& amount);
    void translate(const Vector3D& amount);

    // Returns true if and only if this node is a JointNode
    virtual bool is_joint() const;
  
protected:
    // Useful for picking
    int m_id;
    std::string m_name;

    // Transformations
    Matrix4x4 m_trans;
    Matrix4x4 m_inv;

    // Hierarchy
    typedef std::list<SceneNode*> ChildList;
    ChildList m_children;
};

class JointNode : public SceneNode {
public:
    JointNode(const std::string& name);
    virtual ~JointNode();

    virtual bool is_joint() const;

    void set_joint_x(double min, double init, double max);
    void set_joint_y(double min, double init, double max);

    struct JointRange {
        double min, init, max;
    };


protected:
    JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
    GeometryNode(const std::string& name,
               Primitive* primitive);
    virtual ~GeometryNode();

    virtual bool exists_intersection(const Ray& ray);
    virtual bool get_intersection(const Ray& ray, Intersection* isect);

    const Material* get_material() const;
    Material* get_material();

    const Primitive* get_primitive() const;
    Primitive* get_primitive();

    void set_material(Material* material)
    {
        m_material = material;
    }

protected:
    Material* m_material;
    Primitive* m_primitive;
};

#endif
