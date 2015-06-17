/*
 * SphereMesh.cpp
 * Written by: Spencer Van Leeuwen
 * ID: 20412199
 */

#include<math.h>
#include<stack>
#include<iostream>
#include "sphere.hpp"

using namespace std;

#define PI 3.14159265

Point::Point(float x, float y, float z)
    : QVector3D(x, y, z) 
{
    mUpdatedPoint = NULL;    
}

void Point::addTriangle(Triangle* tri) {
    mTriangles.push_back(tri);

    vector<Point*>& verts =  tri->getVertices();
        
    for(uint j = 0; j < verts.size(); j++) {
        if(verts.at(j) != this) {
            mNeighbours.insert(verts.at(j));
        }
    }
}

void Point::removeTriangle(Triangle* tri) {
    for(auto it = mTriangles.begin(); it != mTriangles.end(); it++) {
        if(*it == tri) {
            mTriangles.erase(it);
            break;
        }
    }
}

vector<Triangle*>& Point::getTriangles() {
    return mTriangles;
}

int Point::triangleCount() {
    return mTriangles.size();
}

set<Point*>& Point::getNeighbours() {
    return mNeighbours;
}

Point* Point::getUpdate() {
    if(mUpdatedPoint == NULL) {
        int n = mNeighbours.size();
        float beta = getBeta();

        QVector3D point = (1 - n*beta)*((QVector3D)(*this));

        for(auto it = mNeighbours.begin(); it != mNeighbours.end(); it++) {
            point += beta * (QVector3D)(**it);
        }

        mUpdatedPoint = new Point(point.x(), point.y(), point.z());

    }

    return mUpdatedPoint;
}

float Point::getBeta() {
    int n = mNeighbours.size();
    return (4.0 - 2.0*cos(2.0*PI/n))/(9.0*n);
}

Triangle::Triangle(Point* p1, Point* p2, Point* p3) {
    mVertices.push_back(p1);
    mVertices.push_back(p2);
    mVertices.push_back(p3);

    p1->addTriangle(this);
    p2->addTriangle(this);
    p3->addTriangle(this);

    mCenter = NULL;
}

Triangle::~Triangle() {
    while(mVertices.size() > 0) {
        Point* p = mVertices.back();
        p->removeTriangle(this);

        mVertices.pop_back();
        
        if(p->triangleCount() == 0) {
            delete p;
        }
    }

    for(auto it = mNeighbours.begin(); it != mNeighbours.end(); it++) {
        (*it)->removeNeighbour(this);
    }
}

Point* Triangle::getCenter() {
    if(mCenter == NULL) {
        float x = (mVertices.at(0)->x() + mVertices.at(1)->x() + mVertices.at(2)->x()) / 3.0f;
        float y = (mVertices.at(0)->y() + mVertices.at(1)->y() + mVertices.at(2)->y()) / 3.0f;
        float z = (mVertices.at(0)->z() + mVertices.at(1)->z() + mVertices.at(2)->z()) / 3.0f;

        mCenter = new Point(x, y, z);
    }
    
    return mCenter;
}

void Triangle::addNeighbour(Triangle* tri) {
    if(tri != this){
        mNeighbours.insert(tri);    
    }
}

void Triangle::removeNeighbour(Triangle* tri) {
   for(auto it = mNeighbours.begin(); it != mNeighbours.end(); it++) {
        if(*it == tri) {
            mNeighbours.erase(it);
            break;
        }
    }
}

vector<Point*>& Triangle::getVertices() {
    return mVertices;
}

set<Triangle*>& Triangle::getNeighbours() {
    return mNeighbours;
}

bool Triangle::shareEdge(Triangle* t1, Triangle* t2) {
    int count = 0;

    vector<Point*>& verts1 = t1->getVertices();
    vector<Point*>& verts2 = t2->getVertices();

    for(uint i = 0; i < verts1.size(); i++) {
        for(uint j = 0; j < verts2.size(); j++) {
            if(verts1.at(i) == verts2.at(j)) {
                 count++;
            }
        }
    }

    return count == 2;
}

Triangle* Triangle::getNeighbourWithEdge(Point* p1, Point* p2) {
    for(auto it = mNeighbours.begin(); it != mNeighbours.end(); it++) {
        vector<Point*>& verts = (*it)->getVertices();

        bool hasP1 = false;
        bool hasP2 = false;

        for(uint i = 0; i < verts.size(); i++) {
            if(p1 == verts.at(i)) {
                hasP1 = true;
            } else if(p2 == verts.at(i)) {
                hasP2 = true;
            }
        }    
        
        if(hasP1 && hasP2) {
            return *it;
        }
    }

    return NULL;
}

SphereMesh::SphereMesh(int subdivisions) {
    Point* p1 = new Point(1.0f, 0, -1.0f/sqrt(2));
    Point* p2 = new Point(-1.0f, 0, -1.0f/sqrt(2));
    Point* p3 = new Point(0, 1.0f, 1.0f/sqrt(2));
    Point* p4 = new Point(0, -1.0f, 1.0f/sqrt(2));

    mFaces = new vector<Triangle*>();

    mFaces->push_back(new Triangle(p1, p2, p3));
    mFaces->push_back(new Triangle(p1, p4, p2));
    mFaces->push_back(new Triangle(p2, p4, p3));
    mFaces->push_back(new Triangle(p1, p3, p4));
   
    for(uint i = 0; i < mFaces->size(); i++) {
        for(uint j = i+1; j < mFaces->size(); j++) {
            mFaces->at(i)->addNeighbour(mFaces->at(j));
            mFaces->at(j)->addNeighbour(mFaces->at(i));
        } 
    }

    for(int i = 0; i < subdivisions; i++) {
        subdivide();
    }
}
// After adding a triangle to a vector of faces, we want to add its neighbours that
// are already in the vector
void connectBackFace(vector<Triangle*>* faces) {
    Triangle* tri = faces->at(faces->size() - 1);
    vector<Point*>& verts = tri->getVertices();

    for(uint i = 0; i < faces->size() - 1; i++) {
        for(uint j = 0; j < verts.size(); j++) {
            vector<Triangle*>& triangles = verts.at(j)->getTriangles();
            
            for(uint k = 0; k < triangles.size(); k++) {
                if(Triangle::shareEdge(tri, triangles.at(k))) {
                    tri->addNeighbour(triangles.at(k));
                    triangles.at(k)->addNeighbour(tri);
                }
            }
        }
    }
}

void SphereMesh::subdivide() {
    vector<Triangle*>* newFaces = new vector<Triangle*>();

    set<Triangle*> oldFaces;
    oldFaces.insert(mFaces->at(0));
    mFaces = NULL;

    while(oldFaces.size() > 0) {
        Triangle* face = *(oldFaces.begin());
        oldFaces.erase(oldFaces.begin());

        vector<Point*>& verts = face->getVertices();
        Point* center = face->getCenter();

        set<Triangle*>& neighbours = face->getNeighbours();
        for(auto it = neighbours.begin(); it != neighbours.end(); it++) {
            oldFaces.insert(*it);
        }
        for(uint j = 0; j < 3; j++) {
            int a = j; 
            int b = (j+1) % 3;

            Triangle* neigh = face->getNeighbourWithEdge(verts.at(a), verts.at(b));

            for(auto it = neighbours.begin(); it != neighbours.end(); it++) {
                if((*it) == neigh) {
                    newFaces->push_back(new Triangle(verts.at(a)->getUpdate(), (*it)->getCenter(), center));
                    connectBackFace(newFaces);
                    newFaces->push_back(new Triangle(center, (*it)->getCenter(), verts.at(b)->getUpdate()));
                    connectBackFace(newFaces);
                    break;
                }
            }
        }

        delete face;
    }
    mFaces = newFaces;
}

vector<Triangle*>* SphereMesh::getFaces() {
    return mFaces;
}
