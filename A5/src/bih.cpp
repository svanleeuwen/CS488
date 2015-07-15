#ifdef BIH
#include "bih.hpp"
#include "algebra.hpp"

#include <iostream>
#include <stdlib.h>
#include <stack>

using std::vector;
using std::cerr;
using std::cout;
using std::endl;
using std::stack;

#define MAX_DEPTH 40

// ********************** BIHTree *****************************

BIHTree::BIHTree(Primitive** primitives, int size) :
    m_primitives(primitives), m_numPrimitives(size)
{
    initGlobalBBox();
    m_root = new BIHNode(m_primitives, size, m_globalBBox);

    stack<BIHNode*>* nodes = new stack<BIHNode*>();
    stack<AABB>* bboxes = new stack<AABB>();
    
    m_root->buildHierarchy(nodes, bboxes, m_globalBBox);

    while(nodes->size() > 0) {
        BIHNode* node = nodes->top();
        nodes->pop();
        AABB bbox = bboxes->top();
        bboxes->pop();

        node->buildHierarchy(nodes, bboxes, bbox);
    }

    delete nodes;
    delete bboxes;
}

BIHTree::~BIHTree() {
    delete m_root;
}

bool BIHTree::getIntersection(const Ray& ray, Intersection* isect) {
    bool hit = m_root->getIntersection(ray, isect);
    return hit;
}

namespace {
    struct Node{
        Node(BIHNode* node, int firstActive) :
            m_node(node), m_firstActive(firstActive) {}

        BIHNode* m_node;
        int m_firstActive;
    };
}

void BIHTree::getIntersection(Packet& packet, vector<bool>& v_hit, vector<Intersection>* v_isect) {
    BIHNode* node = m_root;
    int firstActive = 0;
   
    vector<Ray>* rays = &packet.m_rays;
    int n = rays->size();

    stack<Node> hitNodes;

    for(int i = 0; i < n; i++) {
        v_hit.at(i) = false;
    }

    while(true) {
        firstActive = node->m_bbox.packetTest(packet, firstActive);

        if(firstActive < n) {
            if(node->m_type != BIHNode::Type::leaf) {
                int first = node->traversalOrder(rays->at(firstActive));
                hitNodes.push(Node(node->m_children + (1 - first), firstActive));

                node = node->m_children + first;
                continue;

            } else {
                for(int i = 0; i < node->m_numPrimitives; i++) {
                    if(!(v_isect == NULL && v_hit.at(i))) {
                        node->m_primitives[i]->getIntersection(packet, firstActive, v_hit, v_isect);
                    }
                }
            }
        }

        if(hitNodes.empty()) {
            return;
        }

        if(v_isect == NULL) {
            bool allHit = true;

            for(int i = 0; i < n; i++) {
                if(!v_hit.at(i)) {
                    allHit = false;
                    break;
                }
            }

            if(allHit) {
                return;
            }
        }

        Node next = hitNodes.top();
        hitNodes.pop();

        node = next.m_node;
        firstActive = next.m_firstActive;
    }
}

void BIHTree::initGlobalBBox() {
    AABB* bbox = m_primitives[0]->getWorldBBox();

    Point3D min = bbox->m_min;
    Point3D max = bbox->m_max; 
    
    for(int i = 0; i < m_numPrimitives; ++i) {
        bbox = m_primitives[i]->getWorldBBox();

        min = Point3D::min(min, bbox->m_min);
        max = Point3D::max(max, bbox->m_max);
    }

    m_globalBBox = AABB(min, max);
}

// ********************** BIHNode *****************************

BIHNode::BIHNode(Primitive** primitives, int size, const AABB& bbox, int depth) :
    m_primitives(primitives), m_numPrimitives(size), m_bbox(bbox), m_depth(depth)
{
    m_type = Type::leaf;
}

BIHNode::~BIHNode() {
    if(m_type != Type::leaf) {
        delete[] m_children;
        delete[] m_planes;
    }
}

void BIHNode::buildHierarchy(stack<BIHNode*>* nodes, stack<AABB>* bboxes, const AABB& uniformBBox) {
    if(m_type != Type::leaf) {
        cerr << "Trying to expand an inner node" << endl;
        exit(1);
    }

    m_type = chooseAxis(uniformBBox);
    int axis = (int)m_type;

    double median = uniformBBox.getMedian(axis);

    int i = 0;
    int j = m_numPrimitives - 1;

    double leftMax = m_bbox.m_min[axis];
    double rightMin = m_bbox.m_max[axis];

    while(i <= j) {
        AABB* i_bbox = m_primitives[i]->getWorldBBox();
        double i_mid = i_bbox->getMedian(axis);

        if(i_mid <= median) {
            ++i;
            leftMax = fmax(i_bbox->m_max[axis], leftMax);
            continue;
        }

        while(i <= j) {
            AABB* j_bbox = m_primitives[j]->getWorldBBox();
            double j_mid = j_bbox->getMedian(axis);

            if(j_mid > median) {
                --j;
                rightMin = fmin(j_bbox->m_min[axis], rightMin);
            
            } else {
                Primitive* temp = m_primitives[i];
                m_primitives[i] = m_primitives[j];
                m_primitives[j] = temp;

                break;
            }
        }
    }
   
    Primitive** primitives = m_primitives;
    int numPrimitives = m_numPrimitives;
    
    m_children = new BIHNode[2] 
        { BIHNode(primitives, i, getLeftBBox(m_bbox, leftMax), m_depth + 1), 
            BIHNode(primitives + i, numPrimitives - i, getRightBBox(m_bbox, rightMin), m_depth + 1) };
    
    m_planes = new double[2] { leftMax, rightMin };    

    if(i > 1 && m_depth < MAX_DEPTH) {
        AABB l_bbox = getLeftBBox(uniformBBox, median);
        nodes->push(m_children);
        bboxes->push(l_bbox);
    }

    if(numPrimitives - i > 1 && m_depth < MAX_DEPTH) {
        AABB r_bbox = getRightBBox(uniformBBox, median);
        nodes->push(m_children + 1);
        bboxes->push(r_bbox);
    }
}

int BIHNode::traversalOrder(const Ray& ray) {
    Vector3D direction = ray.getDirection();
    return (direction[(int)m_type] > 0) ? 0 : 1;
}

bool BIHNode::getIntersection(const Ray& ray, Intersection* isect) {
    if( !(m_bbox.intersect(ray) || m_bbox.contains(ray)) ) {
        return false;

    } else if(m_type == Type::leaf) {
        return getLeafIntersection(ray, isect);
    }   
    
    Vector3D direction = ray.getDirection();

    int first = (direction[(int)m_type] > 0) ? 0 : 1;

    Intersection* t_isect = (isect == NULL) ? NULL : new Intersection();
    bool hit = m_children[first].getIntersection(ray, t_isect);

    bool hitAny = hit;
    Ray newRay = (hit && isect != NULL) ? Ray(ray.getOrigin(), t_isect->getPoint()) : ray;
    
    hit = m_children[(first+1)%2].getIntersection(newRay, t_isect);

    hitAny = hitAny || hit;

    if(isect != NULL) { 
        if(hitAny) {
            *isect = *t_isect;
        }
        delete t_isect;
    }

    return hitAny;
}

bool BIHNode::getLeafIntersection(const Ray& ray, Intersection* isect) {
    Ray testRay = ray;

    Intersection* best = (isect == NULL) ? NULL : new Intersection();
    bool hitAny = false;

    for(int i = 0; i < m_numPrimitives; i++) {
        bool hit = m_primitives[i]->getIntersection(testRay, best);

        if(isect != NULL && hit) {
            hitAny = true;
            testRay = Ray(testRay.getOrigin(), best->getPoint());

        } else if(hit) {
            return true;
        }
    }

    if(isect != NULL) {
        if(hitAny) {
            *isect = *best;
        }
        delete best;
    }

    return hitAny;
} 

BIHNode::Type BIHNode::chooseAxis(const AABB& bbox) {
    double max = bbox.m_max[0] - bbox.m_min[0];
    Type axis = Type::x_axis;

    double next = bbox.m_max[1] - bbox.m_min[1];
    if(next > max) {
        max = next;
        axis = Type::y_axis;
    }

    next = bbox.m_max[2] - bbox.m_min[2];
    if(next > max) {
        max = next;
        axis = Type::z_axis;
    }

    return axis;
}

AABB BIHNode::getLeftBBox(const AABB& bbox, double plane) {
    if(m_type == Type::leaf) {
        cerr << "Trying to get left bbox of leaf" << endl;
        exit(1);
    }
    
    int axis = (int)m_type;
    
    Point3D max = bbox.m_max;
    max[axis] = plane;
    return AABB(bbox.m_min, max);
}

AABB BIHNode::getRightBBox(const AABB& bbox, double plane) {
    if(m_type == Type::leaf) {
        cerr << "Trying to get right bbox of leaf" << endl;
        exit(1);
    }
    
    int axis = (int)m_type;
 
    Point3D min = bbox.m_min;
    min[axis] = plane;
    return AABB(min, bbox.m_max);
}
#endif
