// Spencer Van Leeuwen
// 20412199

#include <QtGui>
#include <cmath>
#include <iostream>
#include <QElapsedTimer>

#include "paintcanvas.hpp"

using std::list;
using std::vector;
using std::string;

using std::cerr;
using std::cout;
using std::endl;

PaintCanvas::PaintCanvas(QWidget *parent, Camera* cam, const list<Light*>* lights, Colour ambient, 
        vector<Primitive*>* primitives, string& filename) :
    QWidget(parent), m_initCam(cam), m_lights(lights), 
    m_ambient(ambient), m_primitives(primitives), m_filename(QString(filename.c_str()))
{
    m_cam = new Camera(*cam);
    m_cam->updateDimensions(width(), height());

    m_tracer = new Tracer(m_primitives, ambient, lights);
    
    m_img = new QImage(width(), height(), QImage::Format_RGB32);
    m_packets = CameraPacket::genPackets(m_img, m_tracer, *m_cam);

    m_newFrame = false;

    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(75);
    connect(m_resizeTimer, SIGNAL(timeout()), this, SLOT(resizeAction()));
}

PaintCanvas::~PaintCanvas() {
}

QSize PaintCanvas::minimumSizeHint() const {
    return QSize(50, 50);
}   

QSize PaintCanvas::sizeHint() const {
    return QSize(300, 300);
}

void PaintCanvas::saveImage() {
    Camera* t_cam = new Camera(*m_cam);
    t_cam->updateDimensions(m_initCam->getWidth(), m_initCam->getHeight());

    QImage img(m_initCam->getWidth(), m_initCam->getHeight(), QImage::Format_RGB32);
    
    vector<CameraPacket*>* t_packets = m_packets;
    m_packets = CameraPacket::genPackets(&img, m_tracer, *t_cam);

    computeQImage();
    img.save(m_filename);

    delete t_cam;

    CameraPacket::deletePackets(m_packets);
    m_packets = t_packets;
}

void PaintCanvas::resizeAction() {
    m_cam->updateDimensions(width(), height());

    delete m_img;
    m_img = new QImage(width(), height(), QImage::Format_RGB32);

    CameraPacket::deletePackets(m_packets);
    m_packets = CameraPacket::genPackets(m_img, m_tracer, *m_cam);

    m_newFrame = true;
    update();
}

void PaintCanvas::paintEvent(QPaintEvent* event) {
    (void) event;

    if(m_newFrame) {
        computeQImage();
        m_newFrame = false;
    }

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), *m_img);
}

void PaintCanvas::resizeEvent(QResizeEvent* event) {
    (void) event;

    m_resizeTimer->start();
}

void PaintCanvas::computeQImage() {
    QElapsedTimer timer;
    timer.start();

    m_index = 0;
    m_k = 1;

    pthread_mutex_init(&m_mutex, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    cout << "0\% complete" << endl;

    for(int a = 0; a < NUMTHREADS; a++) {
        pthread_create(&m_threads[a], &attr, thread_bootstrap, this);
    }

    pthread_attr_destroy(&attr);
    for(int a = 0; a < NUMTHREADS; a++) {
        int rc = pthread_join(m_threads[a], NULL);
        if(rc) {
            cerr << "ERROR. Return code from pthread_join() is "
                << rc << endl;
            exit(-1);
        }
    }

    cout << "100\% complete" << endl;

    pthread_mutex_destroy(&m_mutex);

    cout << "Time to compute image: " << timer.elapsed() << endl;
    timer.invalidate();
}

void* PaintCanvas::thread_bootstrap(void* canvas) {
    ((PaintCanvas*)canvas)->computePixels();

    return NULL;
}

void PaintCanvas::computePixels() {
    int numPackets = m_packets->size();
    int index;

    while(true) {
        pthread_mutex_lock(&m_mutex);
        
        if(m_index >= numPackets) {
            pthread_mutex_unlock(&m_mutex);
            pthread_exit((void*) 0);
        }

        index = m_index++; 

        pthread_mutex_unlock(&m_mutex);

        m_packets->at(index)->trace();

        if(index == (int) ((m_k/10.0) * numPackets)) {
            cout << m_k*10 << "\% complete" << endl;
            m_k++;
        }
    }       
}
