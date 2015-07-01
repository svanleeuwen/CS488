#include "bih.hpp"
#include "algebra.hpp"

using std::vector;

// ********************** BIHTree *****************************

BIHTree::BIHTree(vector<Primitive*>* primitives) :
    m_primitives(primitives)
{
    initGlobalBBox();
    m_root = new BIHLeaf(m_primitives, 0, m_primitives->size());
}

BIHTree::~BIHTree() {}

bool BIHTree::getIntersection(Ray& ray, Intersection* isect) {
    (void) ray;
    (void) isect;

    // TODO:: traverse BIH
    return false;
}

void BIHTree::initGlobalBBox() {
    AABB* bbox = m_primitives->at(0)->getWorldBBox();

    Point3D min = bbox->m_min;
    Point3D max = bbox->m_max; 
    
    for(auto it = m_primitives->begin() + 1; it != m_primitives->end(); ++it) {
        bbox = (*it)->getWorldBBox();

        min = Point3D::min(min, bbox->m_min);
        max = Point3D::max(max, bbox->m_max);
    }

    m_globalBBox = AABB(min, max);
}

// ********************** BIHNode *****************************

BIHNode::BIHNode() {}

BIHNode::~BIHNode() {}

// ********************** BIHInner ****************************

BIHInner::BIHInner() {}

BIHInner::~BIHInner() {}

// ********************** BIHLeaf *****************************

BIHLeaf::BIHLeaf(std::vector<Primitive*>* primitives, int firstIndex, int numPrimitives) :
    m_primitives(primitives), m_firstIndex(firstIndex), m_numPrimitives(numPrimitives)
{}

BIHLeaf::~BIHLeaf() {}

