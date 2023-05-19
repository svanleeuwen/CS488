#include <iostream>
#include "packet.hpp"
#include "tracer.hpp"
#include "a4.hpp"

using std::vector;
using std::cout;
using std::endl;
using std::max;

int Packet::SAMPLE_WIDTH = 1;
#define PACKET_WIDTH 16

//***************************** Packet *********************************
Packet::Packet() 
{
    m_rays = new std::vector<Ray*>();
}

Packet::~Packet() {
    for(auto it = m_rays->begin(); it != m_rays->end(); ++it) {
        if(*it != NULL) {
            delete *it;
        }
    }

    delete m_rays;
}

vector<Ray*>* copyRays(vector<Ray*>* rays) {
    vector<Ray*>* newRays = new vector<Ray*>();
    for(auto it = rays->begin(); it != rays->end(); ++it) {
        newRays->push_back(new Ray(**it));
    }

    return newRays;
}

void Packet::copy(const Packet& other) {
    m_origin = other.m_origin;
    m_direction = other.m_direction;

    m_dirReciproc = other.m_dirReciproc;

    m_finite = other.m_finite;
    m_length = other.m_length;

    m_rays = copyRays(other.m_rays);
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

void Packet::updateIntervals() {
    m_origin = IVector3D();
    m_direction = IVector3D();

    m_finite = true;
    m_length = 0;

    for(auto it = m_rays->begin(); it != m_rays->end(); ++it) {
        if(*it != NULL) {
            Point3D o = (*it)->getOrigin();
            Vector3D d = (*it)->getDirection();

            m_origin.extend(o);
            m_direction.extend(d);

            if((*it)->hasEndpoint() && m_finite) {
                m_length = max(m_length, (*it)->getLength());
            } else {
                m_finite = false;
                m_length = std::numeric_limits<double>::infinity();
            }
        }
    }

    m_dirReciproc = m_direction.reciprocal();
}

void Packet::setRays(vector<Ray*>* rays) {
    for(auto it = m_rays->begin(); it != m_rays->end(); ++it) {
        delete *it;
    }

    delete m_rays;
    m_rays = rays;

    updateIntervals();
}

//************************** CameraPacket ******************************

CameraPacket::CameraPacket() {}

CameraPacket::CameraPacket(int width, int height, int i, int j, QImage* img, Tracer* tracer) :
    m_width(width), m_height(height), m_i(i), m_j(j), m_img(img),
    m_tracer(tracer)
{
}

CameraPacket::~CameraPacket() {}

void CameraPacket::copy(const CameraPacket& other) {
    m_sampleWidth = other.m_sampleWidth;
   
    m_width = other.m_width;
    m_height = other.m_height;

    m_i = other.m_i;
    m_j = other.m_j;

    m_img = other.m_img;
    m_tracer = other.m_tracer;
}

CameraPacket::CameraPacket(const CameraPacket& other) : 
    Packet(other)
{
    copy(other);
}

CameraPacket& CameraPacket::operator=(const CameraPacket& other) {
    if(this != &other) {
        copy(other);
        Packet::operator=(other);
    }

    return *this;
}

void CameraPacket::genRays(const Camera& cam) {
    int packetWidth = SAMPLE_WIDTH * m_width;
    int packetHeight = SAMPLE_WIDTH * m_height;

    double pixelFraction = 1.0 / SAMPLE_WIDTH;
    
    double starting_i = m_i - 0.5 * (SAMPLE_WIDTH - 1) * pixelFraction;
    double j = m_j - 0.5 * (SAMPLE_WIDTH - 1) * pixelFraction;

    vector<Ray*>* rays = new vector<Ray*>();
    
    for(int y = 0; y < packetHeight; y++) {
        double i = starting_i;

        for(int x = 0; x < packetWidth; x++) {
            rays->push_back(cam.getRay(i, j));
            
            i += pixelFraction;
        }
        
        j += pixelFraction;
    }

    setRays(rays);
}

void CameraPacket::trace() {
    if(PACKETS) {
        int n = m_rays->size();

        ColourVector colours(n);
        vector<bool> v_hit(n);

        Packet packet(*this);
        m_tracer->tracePacket(packet, &colours, v_hit);

        for(int j = 0; j < m_height; j++) {
            for(int i = 0; i < m_width; i++) {
                tracePixel(i, j, colours, v_hit);
            }
        }

    } else {
        for(int j = 0; j < m_height; j++) {
            for(int i = 0; i < m_width; i++) {
                tracePixel(i, j);
            }
        }
    }
}

void CameraPacket::updateIntervals() {
    int packetWidth = SAMPLE_WIDTH * m_width;
    int packetHeight = SAMPLE_WIDTH * m_height;

    if(packetWidth * packetHeight <= 1) {
        Packet::updateIntervals();
    }

    m_origin = IVector3D(m_rays->at(0)->getOrigin());
    m_direction = IVector3D();

    m_finite = false;
    m_length = std::numeric_limits<double>::infinity();

    int indices[4] = {0, packetWidth-1, packetWidth*packetHeight - 1, packetHeight * (packetWidth - 1) + 1};

    for(int i = 0; i < 4; ++i) {
        Ray* ray = m_rays->at(indices[i]);
        Point3D o = ray->getOrigin();
        Vector3D d = ray->getDirection();

        m_origin.extend(o);
        m_direction.extend(d);
    }

    m_dirReciproc = m_direction.reciprocal();
}

void CameraPacket::tracePixel(int i, int j) {
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
            bool hit = m_tracer->traceRay(*m_rays->at(index), colour);

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

void CameraPacket::tracePixel(int i, int j, ColourVector& colours, vector<bool>& v_hit) {
    int width = m_img->width();
    int height = m_img->height();

    int packetWidth = m_width * SAMPLE_WIDTH;

    int img_i = m_i + i;
    int img_j = m_j + j;

    Colour averageColour;
   
#ifdef BLACK_BACKGROUND 
    Colour backgroundColour;
#else
    Colour backgroundColour( ((double)img_i)/(double)width, ((double)img_j)/(double)height, 
       (1.0 - ((double)img_i)/(double)width) );
#endif

    for(int y = 0; y < SAMPLE_WIDTH; y++) {
        for(int x = 0; x < SAMPLE_WIDTH; x++) {
            int index = packetWidth * ((SAMPLE_WIDTH * j) + y) + SAMPLE_WIDTH * i + x;
            bool hit = v_hit.at(index);

            if(hit) {
                averageColour += colours.at(index);
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
vector<CameraPacket*>* CameraPacket::genPackets(QImage* img, Tracer* tracer, const Camera& cam, int sampleWidth) {
    CameraPacket::SAMPLE_WIDTH = sampleWidth;

    int width = img->width();
    int height = img->height();

    int std_pixelWidth = PACKET_WIDTH / SAMPLE_WIDTH;

    vector<CameraPacket*>* packets = new vector<CameraPacket*>();

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

            CameraPacket* newPacket = new CameraPacket(pixelWidth, pixelHeight, i, j, img, tracer);
            newPacket->genRays(cam);

            packets->push_back(newPacket);
        }
    }

    return packets;
}

void CameraPacket::deletePackets(vector<CameraPacket*>* packets) {
    for(auto it = packets->begin(); it != packets->end(); it++) {
        delete (*it);
    }

    delete packets;
}
