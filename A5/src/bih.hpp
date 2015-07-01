#ifndef CS488_BIH_HPP
#define CS488_BIH_HPP

#include "primitive.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#include <vector>

class BIHNode;

class BIHTree {
public:
    BIHTree(std::vector<Primitive*>* primitives);
    virtual ~BIHTree();

    bool getIntersection(Ray& ray, Intersection* isect);

private:
    void initGlobalBBox();

    BIHNode* m_root;
    AABB m_globalBBox;
   
    std::vector<Primitive*>* m_primitives;
};

class BIHNode {
public:
    BIHNode();
    virtual ~BIHNode();

    virtual bool isLeaf() { return false; }
};

class BIHInner: public BIHNode {
public:
    BIHInner();
    virtual ~BIHInner();
    
private:
    enum Axis {
        x_axis,
        y_axis,
        z_axis,
    };

    Axis m_axis;

    BIHNode* m_lchild;
    BIHNode* m_rchild;

    double m_planes[2];
};

class BIHLeaf : public BIHNode {
public: 
    BIHLeaf(std::vector<Primitive*>* primitives, int firstIndex, int numPrimitives);
    virtual ~BIHLeaf();

    virtual bool isLeaf() { return true; }

private:
    std::vector<Primitive*>* m_primitives;
    int m_firstIndex;
    int m_numPrimitives;
};

#endif
