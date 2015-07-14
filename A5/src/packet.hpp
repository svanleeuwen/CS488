#ifndef CS488_PACKET_HPP
#define  CS488_PACKET_HPP

#include<vector>
#include<QImage>

#include "ray.hpp"
#include "interval.hpp"
#include "camera.hpp"

class Tracer;

class Packet {
public:
    Packet() {}
    Packet(int width, int height, int i, int j, QImage* img, Tracer* tracer);

    ~Packet();

    Packet(const Packet& other);
    Packet& operator=(const Packet& other);

    IVector3D getOrigin() const { return m_origin; }
    IVector3D getDirection() const { return m_direction; }
    IVector3D getRatio() const { return m_ratio; }

    bool isFinite() const { return m_finite; }
    double getLength() const { return m_length; }

    void genRays(const Camera& cam);
    void trace();

    // Static functions to help manage vectors of packets
    static std::vector<Packet*>* genPackets(QImage* img, Tracer* tracer, const Camera& cam);
    static void deletePackets(std::vector<Packet*>* packets);

private:
    void copy(const Packet& other);

    void updateRatio(const Point3D& o, const Vector3D& d);
    void updateIntervals();

    void tracePixel(int i, int j);

    IVector3D m_origin;
    IVector3D m_direction;

    IVector3D m_ratio;

    bool m_finite;
    double m_length;

    int m_width;
    int m_height;

    int m_sampleWidth;

    int m_i;
    int m_j;

    QImage* m_img;
    Tracer* m_tracer;

    std::vector<Ray*> m_rays;
};

#endif
