#ifdef BIH
#ifndef CS488_BIH_HPP
#define CS488_BIH_HPP

#include "primitive.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#include <vector>
#include <stack>

class BIHNode;

class BIHTree {
public:
    BIHTree(Primitive** primitives, int size);
    virtual ~BIHTree();

    bool getIntersection(const Ray& ray, Intersection* isect);
    void getIntersection(Packet& packet, std::vector<bool>& v_hit, std::vector<Intersection>* v_isect); 

private:
    void initGlobalBBox();

    BIHNode* m_root;
    AABB m_globalBBox;
   
    Primitive** m_primitives;
    int m_numPrimitives;
};

class BIHNode {
public:
    BIHNode(Primitive** primitives, int size, const AABB& bbox, int depth = 0);
    virtual ~BIHNode();

    void buildHierarchy(std::stack<BIHNode*>* nodes, std::stack<AABB>* bboxes, const AABB& uniformBBox);

    bool getIntersection(const Ray& ray, Intersection* isect);
    int traversalOrder(const Ray& ray);

    enum Type {
        x_axis,
        y_axis,
        z_axis,
        leaf
    };

    Type m_type;

    union {
        BIHNode* m_children;
        Primitive** m_primitives;
    };
   
    union { 
        double* m_planes;
        int m_numPrimitives;
    };

    AABB m_bbox;

private:
    bool getLeafIntersection(const Ray& ray, Intersection* isect);

    Type chooseAxis(const AABB& bbox);
    AABB getLeftBBox(const AABB& bbox, double plane);
    AABB getRightBBox(const AABB& bbox, double plane);

    int m_depth;
};

#endif
#endif
