#include "scene.hpp"
#include <iostream>
#include <limits>
#include <math.h>

using namespace std;

SceneNode::SceneNode(const std::string& name)
  : m_name(name), m_transformed(false)
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

void SceneNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;
    
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv);
    }
}

bool SceneNode::exists_intersection(const Ray& ray) {
    Matrix4x4 eye;
    stack<Matrix4x4>* transStack = new stack<Matrix4x4>();
    transStack->push(eye);

    stack<Matrix4x4>* invStack = new stack<Matrix4x4>();
    invStack->push(eye);

    bool hit = exists_intersection(ray, transStack, invStack);
    
    delete transStack;
    delete invStack;
    return hit;
    
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool hit = (*it)->exists_intersection(ray);

        if(hit) {
            return true;
        }
    }

    return false;
}

bool SceneNode::exists_intersection(const Ray& ray, stack<Matrix4x4>* transStack, stack<Matrix4x4>* invStack) {
    if(m_transformed) {
        Matrix4x4 trans = transStack->top() * m_trans;
        transStack->push(trans);

        Matrix4x4 inv = m_inv * invStack->top();
        invStack->push(inv);
    }
    
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool hit = (*it)->exists_intersection(ray, transStack, invStack);

        if(hit) {
            if(m_transformed) {
                transStack->pop();
                invStack->pop();
            }
            return true;
        }
    }
    
    if(m_transformed) {
        transStack->pop();
        invStack->pop();
    }
    return false;
}

bool SceneNode::get_intersection(const Ray& ray, Intersection* isect) {
        Matrix4x4 eye;
        stack<Matrix4x4>* transStack = new stack<Matrix4x4>();
        transStack->push(eye);

        stack<Matrix4x4>* invStack = new stack<Matrix4x4>();
        invStack->push(eye);

        bool hit = get_intersection(ray, isect, transStack, invStack);
        
        delete transStack;
        delete invStack;
        return hit;
}

bool SceneNode::get_intersection(const Ray& ray, Intersection* isect, stack<Matrix4x4>* transStack, stack<Matrix4x4>* invStack) {
    double closest = numeric_limits<double>::infinity();
    bool hitAny = false;
    Intersection t_isect;

    if(m_transformed) {
        Matrix4x4 trans = transStack->top() * m_trans;
        transStack->push(trans);

        Matrix4x4 inv = m_inv * invStack->top();
        invStack->push(inv);
    }

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool hit = (*it)->get_intersection(ray, &t_isect, transStack, invStack);

        if(hit) {
            hitAny = true;
            
            if(t_isect.getParam() < closest) {
                closest = t_isect.getParam();
                *isect = t_isect;
            }
        }
    }

    if(m_transformed) {
        transStack->pop();
        invStack->pop();
    }

    return hitAny;
}

void SceneNode::rotate(char axis, double angle)
{
    Matrix4x4 rotMat = Matrix4x4::getRotMat(axis, angle);
    Matrix4x4 invMat = Matrix4x4::getRotMat(axis, -angle);

    m_trans = m_trans * rotMat;
    m_inv = invMat * m_inv;

    m_transformed = true;
}

void SceneNode::scale(const Vector3D& amount)
{
    Matrix4x4 scaleMat = Matrix4x4::getScaleMat(amount);
    Matrix4x4 invMat = Matrix4x4::getScaleMat(Vector3D(1.0/amount[0], 1.0/amount[1], 1.0/amount[2]));

    m_trans = m_trans * scaleMat;
    m_inv = invMat * m_inv;

    m_transformed = true;
}

void SceneNode::translate(const Vector3D& amount)
{
    Matrix4x4 transMat = Matrix4x4::getTransMat(amount);
    Matrix4x4 invMat = Matrix4x4::getTransMat(-amount);

    m_trans = m_trans * transMat;
    m_inv = invMat * m_inv;

    m_transformed = true;
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
    ~SceneNode();
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
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
    ~SceneNode();
}

void GeometryNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;

    if(!m_primitive_pushed) {
        m_primitive->setTransform(t_trans, t_inv);
        primitives->push_back(*m_primitive);

        m_primitive_pushed = true;

    } else {
        primitives->push_back(m_primitive->clone());
    }

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv);
    }
}


bool GeometryNode::exists_intersection(const Ray& ray, stack<Matrix4x4>* transStack, stack<Matrix4x4>* invStack) {
    bool hit = SceneNode::exists_intersection(ray, transStack, invStack);   

    if(hit) {
        return true;
    }

    Intersection t_isect;
    Ray modelRay;

    Matrix4x4 trans = transStack->top() * m_trans;
    transStack->push(trans);

    Matrix4x4 inv = m_inv * invStack->top();
    invStack->push(inv);

    modelRay = ray.getTransform(inv);
    hit = m_primitive->getIntersection(modelRay, &t_isect, this);
    
    transStack->pop();
    invStack->pop();

    return hit;
}

bool GeometryNode::get_intersection(const Ray& ray, Intersection* isect, stack<Matrix4x4>* transStack, stack<Matrix4x4>* invStack) {
    Intersection t_isect1;
    bool hit1 = SceneNode::get_intersection(ray, &t_isect1, transStack, invStack); 

    Matrix4x4 trans;
    Matrix4x4 inv;

    if(m_transformed) {
        trans = transStack->top() * m_trans;
        transStack->push(trans);

        inv = m_inv * invStack->top();
        invStack->push(inv);

    } else {
        trans = transStack->top();
        inv = invStack->top();
    }
   
    Ray modelRay = ray.getTransform(inv);

    Intersection t_isect2;
    bool hit2 = m_primitive->getIntersection(modelRay, &t_isect2, this); 

    if(hit1 && hit2) {
        if(t_isect1.getParam() < t_isect2.getParam()) {
            *isect = t_isect1;

        } else {
            Vector3D normal = inv.transpose() * t_isect2.getNormal();
            *isect = Intersection(trans * t_isect2.getPoint(), t_isect2.getParam(), this, normal);
        }

    } else if(hit1) {
        *isect = t_isect1;
        
    } else if(hit2) {
        Vector3D normal = inv.transpose() * t_isect2.getNormal();
        *isect = Intersection(trans * t_isect2.getPoint(), t_isect2.getParam(), this, normal);
   
    } else {
        if(m_transformed) {
            transStack->pop();
            invStack->pop();
        }

        return false;
    }

    if(m_transformed) {
        transStack->pop();
        invStack->pop();
    }

    return true;
}

Material* GeometryNode::get_material() {
    return m_material;
}

Primitive* GeometryNode::get_primitive() {
    return m_primitive;
}
