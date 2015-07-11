#ifndef CS488_PACKET_HPP
#define  CS488_PACKET_HPP

#include<vector>

#include "ray.hpp"
#include "image.hpp"
#include "tracer.hpp"

class Packet {
public:
    Packet() {}
    Packet(int width, int height, int i, int j, Image* img, Tracer* tracer);

    ~Packet();

    Packet(const Packet& other);
    Packet& operator=(const Packet& other);

    void genRays(const Camera& cam);
    void trace();

    // Static functions to help manage vectors of packets
    static std::vector<Packet*>* genPackets(Image* img, Tracer* tracer, const Camera& cam);
    static void deletePackets(std::vector<Packet*>* packets);

private:
    void copy(const Packet& other);
    void tracePixel(int i, int j);

    int m_width;
    int m_height;

    int m_sampleWidth;

    int m_i;
    int m_j;

    Image* m_img;
    Tracer* m_tracer;

    std::vector<Ray*> m_rays;
};

#endif
