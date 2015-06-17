#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>

class Viewer;

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(Viewer* view, bool picking = false) const;

  virtual bool rotate_picked_nodes(double x, double y);
  virtual void toggle_picking(int id);

  virtual void reset_joints();

  virtual void push_rotation();
  virtual bool undo();
  virtual bool redo();

  const QMatrix4x4& get_transform() const { return m_trans; }
  
  void set_transform(const QMatrix4x4& m)
  {
    m_trans = m;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const QVector3D& amount);
  void translate(const QVector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  QMatrix4x4 m_trans;
  QMatrix4x4 m_rot;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(Viewer* view, bool picking = false) const;

  virtual bool rotate_picked_nodes(double x, double y);
  virtual void toggle_picking(int id);

  virtual void reset_joints();

  virtual void push_rotation();
  virtual bool undo();
  virtual bool redo();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

   
protected:

  JointRange m_joint_x, m_joint_y;

  double m_angle_x;
  double m_angle_y;

private:
  void rotate_joint_x(double angle);
  void rotate_joint_y(double angle);

  QMatrix4x4 m_rot_mat;

  std::vector<QVector2D> mRotStack;
  std::vector<QVector2D> mUndoStack;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(Viewer* view, bool picking = false) const;

  virtual void reset_joints();

  virtual bool rotate_picked_nodes(double x, double y);
  virtual void toggle_picking(int id);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

protected:
  Material* m_material;
  Primitive* m_primitive;

private:
  bool m_picked;
};

#endif
