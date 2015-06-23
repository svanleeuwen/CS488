#include "scene.hpp"
#include <iostream>
#include <limits>
#include <math.h>
#include "intersection.hpp"

using namespace std;

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

bool SceneNode::exists_intersection(const Ray& ray) {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool hit = (*it)->exists_intersection(ray);

        if(hit) {
            return true;
        }
    }

    return false;
}

bool SceneNode::get_intersection(const Ray& ray, Intersection* isect) {
    double closest = numeric_limits<double>::infinity();
    bool hitAny = false;
    Intersection t_isect;

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool hit = (*it)->get_intersection(ray, &t_isect);

        if(hit) {
            hitAny = true;
            
            if(t_isect.getParam() < closest) {
                closest = t_isect.getParam();
                *isect = t_isect;
            }
        }
    }

    return hitAny;
}

/*void SceneNode::walk_gl(Viewer* view) const
{
    stack<Matrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view);
    }

    matStack->pop();
}*/

void SceneNode::rotate(char axis, double angle)
{
    Matrix4x4 rotMat = Matrix4x4::getRotMat(axis, angle);
    Matrix4x4 invMat = Matrix4x4::getRotMat(axis, -angle);

    m_trans = rotMat * m_trans;
    m_inv = m_inv * invMat;
}

void SceneNode::scale(const Vector3D& amount)
{
    Matrix4x4 scaleMat = Matrix4x4::getScaleMat(amount);
    Matrix4x4 invMat = Matrix4x4::getScaleMat(Vector3D(1/amount[0], 1/amount[1], 1/amount[2]));

    m_trans = scaleMat * m_trans;
    m_inv = m_inv * invMat;
}

void SceneNode::translate(const Vector3D& amount)
{
    Matrix4x4 transMat = Matrix4x4::getTransMat(amount);
    Matrix4x4 invMat = Matrix4x4::getTransMat(-amount);

    m_trans = transMat * m_trans;
    m_inv = m_inv * invMat;
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

/*void JointNode::walk_gl(Viewer* view) const
{
    stack<Matrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view);
    }

    matStack->pop();
}*/

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
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::exists_intersection(const Ray& ray) {
    Intersection t_isect;
    return m_primitive->getIntersection(ray, &t_isect, this) || SceneNode::exists_intersection(ray);
}

bool GeometryNode::get_intersection(const Ray& ray, Intersection* isect) {
    Intersection t_isect1;
    bool hit1 = m_primitive->getIntersection(ray, &t_isect1, this); 

    Intersection t_isect2;
    bool hit2 = SceneNode::get_intersection(ray, &t_isect2); 

    if(hit1 && hit2) {
        if(t_isect1.getParam() < t_isect2.getParam()) {
            *isect = t_isect1;
        } else {
            *isect = t_isect2;
        }

    } else if(hit1) {
        *isect = t_isect1;

    } else if(hit2) {
        *isect = t_isect2;
    
    } else {
        return false;
    }

    return true;
}

/*void GeometryNode::walk_gl(Viewer* view) const
{
    stack<Matrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans);
  
    m_material->apply_gl(view);

    m_primitive->walk_gl(view);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view);
    }

    matStack->pop();
}*/


Material* GeometryNode::get_material() {
    return m_material;
}

Primitive* GeometryNode::get_primitive() {
    return m_primitive;
}
