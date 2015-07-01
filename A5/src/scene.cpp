#include "scene.hpp"
#include "mesh.hpp"

#include <iostream>
#include <limits>
#include <math.h>

using namespace std;

SceneNode::SceneNode(const std::string& name)
  : m_num_parents(0), m_name(name)
{
}

SceneNode::~SceneNode()
{
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->m_num_parents -= 1;

        if((*it)->m_num_parents == 0) {
            delete (*it);
        }
    }
}

void SceneNode::getPrimitives(vector<Primitive*>* primitives) {
    Matrix4x4 eye;
    getPrimitives(primitives, eye, eye);
}

void SceneNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;
   
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv);
    }
}

void SceneNode::rotate(char axis, double angle)
{
    Matrix4x4 rotMat = Matrix4x4::getRotMat(axis, angle);
    Matrix4x4 invMat = Matrix4x4::getRotMat(axis, -angle);

    m_trans = m_trans * rotMat;
    m_inv = invMat * m_inv;
}

void SceneNode::scale(const Vector3D& amount)
{
    Matrix4x4 scaleMat = Matrix4x4::getScaleMat(amount);
    Matrix4x4 invMat = Matrix4x4::getScaleMat(Vector3D(1.0/amount[0], 1.0/amount[1], 1.0/amount[2]));

    m_trans = m_trans * scaleMat;
    m_inv = invMat * m_inv;
}

void SceneNode::translate(const Vector3D& amount)
{
    Matrix4x4 transMat = Matrix4x4::getTransMat(amount);
    Matrix4x4 invMat = Matrix4x4::getTransMat(-amount);

    m_trans = m_trans * transMat;
    m_inv = invMat * m_inv;
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;

  rotate('x', init);
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;

  rotate('y', init);
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive), m_primitive_pushed(false)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;

    if(!m_primitive_pushed) {
        m_primitive->setTransform(t_trans, t_inv);
        m_primitive->setMaterial(m_material);

        if(!m_primitive->isMesh()) {
            primitives->push_back(m_primitive);
        
        } else {
            ((Mesh*)m_primitive)->addMeshPolygons(primitives);
        }

        m_primitive_pushed = true;

    } else {
        Primitive* prim = m_primitive->clone();
        prim->setTransform(t_trans, t_inv);
        
        if(!prim->isMesh()) {
            primitives->push_back(prim);
        
        } else {
            ((Mesh*)prim)->addMeshPolygons(primitives);
            delete prim;
        }
    }

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv);
    }
}

Material* GeometryNode::get_material() {
    return m_material;
}

Primitive* GeometryNode::get_primitive() {
    return m_primitive;
}
