#include "scene.hpp"
#include "mesh.hpp"

#include <iostream>
#include <limits>
#include <math.h>

using namespace std;

SceneNode::SceneNode(const std::string& name)
  : m_num_parents(0), m_name(name), m_firstRun(true)
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

void SceneNode::getPrimitives(vector<Primitive*>* primitives, Game* game) {
    Matrix4x4 eye;
    getPrimitives(primitives, eye, eye, game);
}

void SceneNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;
   
    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv, game);
    }
}

bool SceneNode::initGame(Game*& game) {
    bool found = false;

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        found = found || (*it)->initGame(game);
    }

    return found;
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
    m_primitive(primitive), m_texture(NULL), m_bump(NULL), m_primitive_pushed(false)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game) {
    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;

    if(!m_primitive_pushed) {
        m_primitive->setMaterial((PhongMaterial*)m_material);
        m_primitive->setTexture(m_texture);
        m_primitive->setBump(m_bump);
        m_primitive_pushed = true;
    }

    Primitive* prim = m_primitive->clone();
    prim->setTransform(t_trans, t_inv);

    if(!prim->isMesh()) {
        primitives->push_back(prim);
    
    } else {
        ((Mesh*)prim)->addMeshPolygons(primitives);
        delete prim;
    }

    for(auto it = m_children.begin(); it != m_children.end(); it++) {
        (*it)->getPrimitives(primitives, t_trans, t_inv, game);
    }
}

Primitive* GeometryNode::get_primitive() {
    return m_primitive;
}

Material* GeometryNode::get_material() {
    return m_material;
}

Texture* GeometryNode::get_texture() {
    return m_texture;
}

Bump* GeometryNode::get_bump() {
    return m_bump;
}

TetrisNode::TetrisNode(const string& name) : 
    SceneNode(name)
{}

TetrisNode::~TetrisNode() 
{}

void TetrisNode::buildBorder(const Matrix4x4& trans, const Matrix4x4& inv) {
    GeometryNode* node = dynamic_cast<GeometryNode*>(m_children.at(0));
    Primitive* prim = node->get_primitive();
    Material* material = node->get_material();
    Texture* texture = node->get_texture();
    Bump* bump = node->get_bump();

    Vector3D transFactors[3] = 
    {
        Vector3D(-6, -11, 0),
        Vector3D(5, -11, 0),
        Vector3D(-6, -11, 0)
    };

    Vector3D scaleFactors[3]
    {
        Vector3D(1, 21, 1),
        Vector3D(1, 21, 1),
        Vector3D(12, 1, 1)
    };

    for(int i = 0; i < 3; ++i) {
        Matrix4x4 transMat = Matrix4x4::getTransMat(transFactors[i]);
        Matrix4x4 i_transMat = Matrix4x4::getTransMat(-transFactors[i]);

        Matrix4x4 scaleMat = Matrix4x4::getScaleMat(scaleFactors[i]);
        Matrix4x4 i_scaleMat = Matrix4x4::getScaleMat(Vector3D(1.0/scaleFactors[i][0], 1.0/scaleFactors[i][1], 1.0/scaleFactors[i][2]));

        Matrix4x4 t_trans = trans * transMat * scaleMat;
        Matrix4x4 t_inv = i_scaleMat * i_transMat * inv;

        Primitive* t_prim = prim->clone();
        t_prim->setMaterial((PhongMaterial*)material);
        t_prim->setTexture(texture);
        t_prim->setBump(bump);
        t_prim->setTransform(t_trans, t_inv);

        m_border.push_back(t_prim);
    }

    delete prim;
}

void TetrisNode::initPieceTypes() {
    for(int i = 1; i < 8; ++i) {
        GeometryNode* node = dynamic_cast<GeometryNode*>(m_children.at(i));

        Primitive* prim = node->get_primitive();
        prim->setMaterial((PhongMaterial*)node->get_material());
        prim->setTexture(node->get_texture());
        prim->setBump(node->get_bump());

        m_pieceTypes.push_back(prim);
    }
}

void TetrisNode::buildPieces(const Matrix4x4& trans, const Matrix4x4& inv, Game* game) {
    deletePieces();

    for(int i = 0; i < game->getHeight(); i++)
    {
        for(int j = 0; j < game->getWidth(); j++)
        {
            int primIndex = game->get(i, j); 
            if(primIndex > -1) {
                Vector3D transFactor = Vector3D(-5 + j, -10 + i, 0);

                Matrix4x4 transMat = trans * Matrix4x4::getTransMat(transFactor);
                Matrix4x4 invMat = Matrix4x4::getTransMat(-transFactor) * inv;
                
                Primitive* prim = m_pieceTypes.at(primIndex)->clone();
                prim->setTransform(transMat, invMat);

                m_pieces.push_back(prim);
            }
        }
    }
}

void TetrisNode::deletePieces() {
/*    for(auto it = m_pieces.begin(); it != m_pieces.end(); ++it) {
        delete *it;
    }*/
    
    m_pieces.clear();
}

void TetrisNode::getPrimitives(vector<Primitive*>* primitives, const Matrix4x4& trans, const Matrix4x4& inv, Game* game) {
    if(game == NULL) {
        return;
    }

    Matrix4x4 t_trans = trans * m_trans;
    Matrix4x4 t_inv = m_inv * inv;

    if(m_children.size() != 8) {
        return;
    }

    // Add Border
    if(m_border.size() == 0) {
        buildBorder(t_trans, t_inv);
    }

    for(auto it = m_border.begin(); it != m_border.end(); ++it) {
        primitives->push_back((*it)->clone());
    }

    // Initialize piece type primitives
    if(m_pieceTypes.size() == 0) {
        initPieceTypes();
    }

    // Add pieces
    buildPieces(t_trans, t_inv, game);
    for(auto it = m_pieces.begin(); it != m_pieces.end(); ++it) {
        primitives->push_back(*it);
    }
}

bool TetrisNode::initGame(Game*& game) {
    game = new Game();
    return true;
}
