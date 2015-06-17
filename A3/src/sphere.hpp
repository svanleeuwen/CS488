/*
 * SphereMesh.hpp
 * Written by: Spencer Van Leeuwen
 * ID: 20412199
 */

#ifndef CS488_SPHERE_HPP
#define CS488_SPHERE_HPP

#include<QVector3D>
#include<set>
#include<vector>

class Triangle;

class Point : public QVector3D {

public:
    Point(float xpos, float ypos, float zpos);

    void addTriangle(Triangle* tri);
    void removeTriangle(Triangle* tri);
    std::vector<Triangle*>& getTriangles();
    
    int triangleCount();

    std::set<Point*>& getNeighbours();

    Point* getUpdate();

private:
    float getBeta();

    std::vector<Triangle*> mTriangles;
    std::set<Point*> mNeighbours;
    Point* mUpdatedPoint;
};

class Triangle {

public:
    // We assume that points are given in counter-clockwise order
    Triangle(Point* p1, Point* p2, Point* p3);
    ~Triangle();

    Point* getCenter();

    void addNeighbour(Triangle* tri);
    void removeNeighbour(Triangle* tri);

    std::vector<Point*>& getVertices();
    std::set<Triangle*>& getNeighbours();

    static bool shareEdge(Triangle* t1, Triangle* t2);

    Triangle* getNeighbourWithEdge(Point* p1, Point* p2);

private:
    std::vector<Point*> mVertices;
    std::set<Triangle*> mNeighbours;
    Point* mCenter;
};

class SphereMesh {

public:
    SphereMesh(int subdivisions = 0);

    // We subdivide the sphere using sqrt(3)-Subdivision
    void subdivide();
    
    std::vector<Triangle*>* getFaces();

private:
    std::vector<Triangle*>* mFaces;
};

#endif
