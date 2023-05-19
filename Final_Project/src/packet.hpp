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
    Packet();
    virtual ~Packet();

    Packet(const Packet& other);
    Packet& operator=(const Packet& other);

    IVector3D getOrigin() const { return m_origin; }
    IVector3D getDirection() const { return m_direction; }
    IVector3D getDirReciproc() const { return m_dirReciproc; }

    bool isFinite() const { return m_finite; }
    double getLength() const { return m_length; }

    void setRays(std::vector<Ray*>* rays);
    std::vector<Ray*>* getRays() { return m_rays; }

protected:
    virtual void updateIntervals();

    std::vector<Ray*>* m_rays;

    IVector3D m_origin;
    IVector3D m_direction;

    IVector3D m_dirReciproc;

    bool m_finite;
    double m_length;

    static int SAMPLE_WIDTH;

private:
    void copy(const Packet& other);
};

class CameraPacket : public Packet{
public:
    CameraPacket();
    CameraPacket(int width, int height, int i, int j, QImage* img, Tracer* tracer);

    virtual ~CameraPacket();

    CameraPacket(const CameraPacket& other);
    CameraPacket& operator=(const CameraPacket& other);

    void genRays(const Camera& cam);
    void trace();
    
    // Static functions to help manage vectors of packets
    static std::vector<CameraPacket*>* genPackets(QImage* img, Tracer* tracer, const Camera& cam, int sampleWidth);
    static void deletePackets(std::vector<CameraPacket*>* packets);

protected:
    virtual void updateIntervals();

private: 
    void copy(const CameraPacket& other);

    void tracePixel(int i, int j);
    void tracePixel(int i, int j, ColourVector& colours, std::vector<bool>& v_hit);

    int m_width;
    int m_height;

    int m_sampleWidth;

    int m_i;
    int m_j;

    QImage* m_img;
    Tracer* m_tracer;
};

#endif
