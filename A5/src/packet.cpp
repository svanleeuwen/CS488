#include <iostream>
#include "packet.hpp"
#include "tracer.hpp"

using std::vector;
using std::cout;
using std::endl;

#define SAMPLE_WIDTH 4
#define PACKET_WIDTH 16

Packet::Packet(int width, int height, int i, int j, QImage* img, Tracer* tracer) :
    m_width(width), m_height(height), m_i(i), m_j(j), m_img(img),
    m_tracer(tracer)
{}

Packet::~Packet() {
    for(auto it = m_rays.begin(); it != m_rays.end(); it++) {
        delete (*it);
    }
}

void Packet::copy(const Packet& other) {
    m_width = other.m_width;
    m_height = other.m_height;

    m_i = other.m_i;
    m_j = other.m_j;
    m_img = other.m_img;
}

Packet::Packet(const Packet& other) {
    copy(other);
}

Packet& Packet::operator=(const Packet& other) {
    if(this != &other) {
        copy(other);
    }

    return *this;
}

void Packet::genRays(const Camera& cam) {
    int packetWidth = SAMPLE_WIDTH * m_width;
    int packetHeight = SAMPLE_WIDTH * m_height;

    double pixelFraction = 1.0 / SAMPLE_WIDTH;
    
    double starting_i = m_i - 0.5 * (SAMPLE_WIDTH - 1) * pixelFraction;
    double j = m_j - 0.5 * (SAMPLE_WIDTH - 1) * pixelFraction;
    
    for(int y = 0; y < packetHeight; y++) {
        double i = starting_i;

        for(int x = 0; x < packetWidth; x++) {
            m_rays.push_back(cam.getRay(i, j));
            
            i += pixelFraction;
        }
        
        j += pixelFraction;
    }

    updateIntervals();
}

void Packet::updateIntervals() {
    m_origin = IVector3D();
    m_direction = IVector3D();
    m_ratio = IVector3D();

    m_finite = false;
    m_length = std::numeric_limits<double>::infinity();

    for(auto it = m_rays.begin(); it != m_rays.end(); ++it) {
        Point3D o = (*it)->getOrigin();
        Vector3D d = (*it)->getDirection();

        m_origin.extend(o);
        m_direction.extend(d);

        updateRatio(o, d);

        if((*it)->hasEndpoint()) {
            m_finite = true;
            m_length = fmin(m_length, (*it)->getLength());
        }
    }
}

void Packet::updateRatio(const Point3D& o, const Vector3D& d) {
    Vector3D v;

    for(int i = 0; i < 3; i++) {
        if(fabs(d[i]) < 1.0e-10) {
            v[i] = d[i] >= 0 ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
        } else {
            v[i] = o[i]/d[i];
        }
    }
        
    m_ratio.extend(v);
}

void Packet::trace() {
    for(int j = 0; j < m_height; j++) {
        for(int i = 0; i < m_width; i++) {
            tracePixel(i, j);
        }
    }
}

void Packet::tracePixel(int i, int j) {
    int width = m_img->width();
    int height = m_img->height();

    int packetWidth = m_width * SAMPLE_WIDTH;

    int img_i = m_i + i;
    int img_j = m_j + j;

    Colour averageColour(0.0, 0.0, 0.0);
   
#ifdef BLACK_BACKGROUND 
    Colour backgroundColour = averageColour;
#else
    Colour backgroundColour( ((double)img_i)/(double)width, ((double)img_j)/(double)height, 
       (1.0 - ((double)img_i)/(double)width) );
#endif

    for(int y = 0; y < SAMPLE_WIDTH; y++) {
        for(int x = 0; x < SAMPLE_WIDTH; x++) {
            int index = packetWidth * ((SAMPLE_WIDTH * j) + y) + SAMPLE_WIDTH * i + x;
            
            Colour colour(0.0, 0.0, 0.0);
            bool hit = m_tracer->traceRay(*(m_rays.at(index)), colour);

            if(hit) {
                averageColour += colour;
            } else {
                averageColour += backgroundColour;
            }
        }
    }

    double factor = 1.0 / (SAMPLE_WIDTH * SAMPLE_WIDTH);
    averageColour = factor * averageColour;

    m_img->setPixel(img_i, img_j, averageColour.toInt());
}

// Static functions to help manage vectors of packets
vector<Packet*>* Packet::genPackets(QImage* img, Tracer* tracer, const Camera& cam) {
    int width = img->width();
    int height = img->height();

    int std_pixelWidth = PACKET_WIDTH / SAMPLE_WIDTH;

    vector<Packet*>* packets = new vector<Packet*>();

    for(int j = 0; j < height; j += std_pixelWidth) {
        int pixelHeight;
        
        if(height - j < std_pixelWidth) {
            pixelHeight = height - j;
        } else {
            pixelHeight = std_pixelWidth;
        }
        
        for(int i = 0; i < width; i += std_pixelWidth) {
            int pixelWidth;

            if(width - i < std_pixelWidth) {
                pixelWidth = width - i;
            } else {
                pixelWidth = std_pixelWidth;
            }

            Packet* newPacket = new Packet(pixelWidth, pixelHeight, i, j, img, tracer);
            newPacket->genRays(cam);

            packets->push_back(newPacket);
        }
    }

    return packets;
}

void Packet::deletePackets(vector<Packet*>* packets) {
    for(auto it = packets->begin(); it != packets->end(); it++) {
        delete (*it);
    }

    delete packets;
}
