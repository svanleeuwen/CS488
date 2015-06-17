#include "scene.hpp"
#include "Viewer.hpp"
#include <iostream>

using namespace std;

namespace {
    int next_id = 0;
}

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
    m_id = next_id++;
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(Viewer* view, bool picking) const
{
    stack<QMatrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans * m_rot);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view, picking);
    }

    matStack->pop();
}

bool SceneNode::rotate_picked_nodes(double x, double y) {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->rotate_picked_nodes(x, y);
    }

    return false;
}

void SceneNode::toggle_picking(int id) {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        if((*it)->is_joint()){
            (*it)->toggle_picking(id);
        }
    }
}

void SceneNode::reset_joints() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->reset_joints();
    }
}

void SceneNode::push_rotation() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->push_rotation();
    }
}

bool SceneNode::undo() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool undone = (*it)->undo();
        
        if(!undone) {
            return false;
        }
    }

    return true;
}

bool SceneNode::redo() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool redone = (*it)->redo();
        
        if(!redone) {
            return false;
        }
    }

    return true;
}

void SceneNode::modelling_rotate(char axis, double angle)
{
    if(axis == 'x') {
        m_trans.rotate(angle, 1.0f, 0.0f, 0.0f);
    } else if(axis == 'y') {
        m_trans.rotate(angle, 0.0f, 1.0f, 0.0f);
    } else {
        m_trans.rotate(angle, 0.0f, 0.0f, 1.0f);
    }
}

void SceneNode::rotate(char axis, double angle)
{
    if(axis == 'x') {
        m_rot.rotate(angle, 1.0f, 0.0f, 0.0f);
    } else if(axis == 'y') {
        m_rot.rotate(angle, 0.0f, 1.0f, 0.0f);
    } else {
        m_rot.rotate(angle, 0.0f, 0.0f, 1.0f);
    }
}

void SceneNode::scale(const QVector3D& amount)
{
    m_trans.scale(amount);
}

void SceneNode::translate(const QVector3D& amount)
{
    m_trans.translate(amount);
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
    m_angle_x = 0;
    m_angle_y = 0;
}

JointNode::~JointNode()
{
}

bool JointNode::rotate_picked_nodes(double x, double y) {
    bool anyChildPicked = false;
    
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        bool childPicked = (*it)->rotate_picked_nodes(x, y);
        
        if(childPicked) {
            anyChildPicked = true;
        }
    }

    if(anyChildPicked) {
        rotate_joint_x(x);
        rotate_joint_y(y);
    }

    mUndoStack.clear();

    return false;
}

void JointNode::toggle_picking(int id) {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->toggle_picking(id);
    }
}

void JointNode::reset_joints() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->reset_joints();
    }

    m_rot.setToIdentity();
    rotate_joint_x(m_joint_x.init);
    rotate_joint_y(m_joint_y.init);

    mRotStack.clear();
    mUndoStack.clear();
}

void JointNode::push_rotation() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->push_rotation();
    }
    
    mRotStack.push_back(QVector2D(m_angle_x, m_angle_y));
    mUndoStack.clear();
}

bool JointNode::undo() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->undo();
    }

    int n = mRotStack.size();
    if(n > 0) {
        QVector2D currentAngle = mRotStack.at(n-1);
        mUndoStack.push_back(currentAngle);
        mRotStack.pop_back();

        QVector2D previousAngle;

        if(n > 1) {
            previousAngle = mRotStack.at(n-2);
        } else {
            previousAngle = QVector2D(m_joint_x.init, m_joint_y.init);
        }


        rotate_joint_y(previousAngle.y() - currentAngle.y());
        rotate_joint_x(previousAngle.x() - currentAngle.x());
        
        return true;
    }
    
    return false;    
}

bool JointNode::redo() {
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->redo();
    }

    int n = mRotStack.size();
    int m = mUndoStack.size();
    if(m > 0) {
        QVector2D currentAngle;
       
        if(n > 0) { 
            currentAngle = mRotStack.at(n-1);
        } else {
            currentAngle = QVector2D(m_joint_x.init, m_joint_y.init);
        }

        QVector2D previousAngle = mUndoStack.at(m-1);
        mRotStack.push_back(previousAngle);
        mUndoStack.pop_back();

        rotate_joint_x(previousAngle.x() - currentAngle.x());
        rotate_joint_y(previousAngle.y() - currentAngle.y());

        return true;
    } 

    return false;
}

void JointNode::walk_gl(Viewer* view, bool picking) const
{
    stack<QMatrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans * m_rot);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view, picking);
    }

    matStack->pop();
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

  m_angle_x = 0;

  rotate_joint_x(init);
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;

  m_angle_y = 0;

  rotate_joint_y(init);
}

double clamp(double min, double val, double max) {
    if(val < min) {
        return min;
    } else if(val > max) {
        return max;
    } else {
        return val;
    }
}

void JointNode::rotate_joint_x(double angle) {
    double oldAngle = m_angle_x;
    m_angle_x = clamp(m_joint_x.min, m_angle_x + angle, m_joint_x.max);

    rotate('x', m_angle_x - oldAngle);
}

void JointNode::rotate_joint_y(double angle) {
    double oldAngle = m_angle_y;
    m_angle_y = clamp(m_joint_y.min, m_angle_y + angle, m_joint_y.max);

    rotate('y', m_angle_y - oldAngle);
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
    m_picked = false;
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::reset_joints() 
{
    m_picked = false;
}

bool GeometryNode::rotate_picked_nodes(double x, double y) {
    (void) x;
    (void) y;

    return m_picked;
}

void GeometryNode::toggle_picking(int id) {
    if(m_id == id) {
        m_picked = !m_picked; 
    }
}

void GeometryNode::walk_gl(Viewer* view, bool picking) const
{
    stack<QMatrix4x4>* matStack = view->mMatStack;
    matStack->push(matStack->top() * m_trans * m_rot);
  
    if(picking) { 
        view->set_colour(
                QColor((m_id & 0x000000FF) >> 0,
                    (m_id & 0x0000FF00) >> 8,
                    (m_id & 0x00FF0000) >> 16));
    } else {
        if(m_picked) {
            view->set_colour(QColor(255, 0, 255));
        } else {
            m_material->apply_gl(view);
        }
    }

    m_primitive->walk_gl(view, picking);

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->walk_gl(view, picking);
    }

    matStack->pop();
}


Material* GeometryNode::get_material() {
    return m_material;
}
