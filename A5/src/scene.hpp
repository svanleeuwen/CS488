#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <stack>
#include <vector>

#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "game.hpp"

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

    void getPrimitives(std::vector<Primitive*>* primitives, Game* game = NULL);
    virtual void getPrimitives(std::vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game);
    
    virtual bool initGame(Game*& game);
    
    // Callbacks to be implemented.
    // These will be called from Lua.
    void rotate(char axis, double angle);
    void scale(const Vector3D& amount);
    void translate(const Vector3D& amount);

    // Returns true if and only if this node is a JointNode
    virtual bool is_joint() const;
 
    int m_num_parents;

protected:
    // Useful for picking
    int m_id;
    std::string m_name;

    // Transformations
    Matrix4x4 m_trans;
    Matrix4x4 m_inv;

    // Hierarchy
    typedef std::vector<SceneNode*> ChildList;
    ChildList m_children;

    bool m_firstRun;
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

    virtual void getPrimitives(std::vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game);

//    virtual bool exists_intersection(const Ray& ray, std::stack<Matrix4x4>* transStack, std::stack<Matrix4x4>* invStack);
//    virtual bool get_intersection(const Ray& ray, Intersection* isect, std::stack<Matrix4x4>* transStack, std::stack<Matrix4x4>* invStack);
    
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

    bool m_primitive_pushed;
};

class TetrisNode : public SceneNode {
public:
    TetrisNode(const std::string& name);
    virtual ~TetrisNode();

    virtual void getPrimitives(std::vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game);

    virtual bool initGame(Game*& game);

private:
    void buildBorder(const Matrix4x4& trans, const Matrix4x4& inv);
    void buildPieces(const Matrix4x4& trans, const Matrix4x4& inv, Game* game);
    
    void initPieceTypes();
    void deletePieces();

    std::vector<Primitive*> m_border;    
    std::vector<Primitive*> m_pieceTypes;
    std::vector<Primitive*> m_pieces;
};

#endif
