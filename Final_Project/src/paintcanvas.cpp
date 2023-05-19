// Spencer Van Leeuwen
// 20412199

#include <QtGui>
#include <cmath>
#include <iostream>
#include <QElapsedTimer>
#include <set>

#include "paintcanvas.hpp"

using std::list;
using std::vector;
using std::set;
using std::string;

using std::cerr;
using std::cout;
using std::endl;

#define FRAMERATE 30

PaintCanvas::PaintCanvas(QWidget *parent, Camera* cam, const list<Light*>* lights, Colour ambient, 
        SceneNode* root, string& filename) :
    QWidget(parent), m_game(NULL), m_printStatus(false),
    m_initCam(cam), m_lights(lights), 
    m_ambient(ambient), m_filename(QString(filename.c_str())), 
    m_root(root), m_refreshScreen(false), m_paused(false), 
    m_piecesMoved(false),
    m_tickCount(0), m_sampleWidth(1)
{
    m_cam = new Camera(*cam);
    m_cam->updateDimensions(width(), height());

    m_root->initGame(m_game);
    
    m_primitives = new vector<Primitive*>();
    m_root->getPrimitives(m_primitives, m_game);

    m_tracer = new Tracer(m_primitives, ambient, lights);

    m_img = new QImage(width(), height(), QImage::Format_RGB32);
    m_packets = CameraPacket::genPackets(m_img, m_tracer, *m_cam, m_sampleWidth);

    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(250);
    connect(m_resizeTimer, SIGNAL(timeout()), this, SLOT(resizeAction()));

    if(m_game != NULL) {
        m_gameTimer = new QTimer(this);
        connect(m_gameTimer, SIGNAL(timeout()), this, SLOT(tick()));
        setTickSpeed(Speed::slow);

        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(refresh()));
        m_updateTimer->start(1000/FRAMERATE);
    }
}

PaintCanvas::~PaintCanvas() {
}

QSize PaintCanvas::minimumSizeHint() const {
    return QSize(50, 50);
}   

QSize PaintCanvas::sizeHint() const {
    return QSize(m_initCam->getWidth(), m_initCam->getHeight());
}

void PaintCanvas::saveImage() {
    pause();

    Camera* t_cam = new Camera(*m_cam);
    t_cam->updateDimensions(m_initCam->getWidth(), m_initCam->getHeight());

    QImage img(m_initCam->getWidth(), m_initCam->getHeight(), QImage::Format_RGB32);
    
    vector<CameraPacket*>* t_packets = m_packets;
    m_packets = CameraPacket::genPackets(&img, m_tracer, *t_cam, m_sampleWidth);

    QElapsedTimer timer;
    timer.start();

    computeQImage();
    img.save(m_filename);

    cout << "Time to save image: " << timer.elapsed() << endl;
    timer.invalidate();

    delete t_cam;

    CameraPacket::deletePackets(m_packets);
    m_packets = t_packets;

    pause();
}

void PaintCanvas::setSampleWidth(int width) {
    m_sampleWidth = width;
    resizeAction();
}

void PaintCanvas::resizeAction() {
    m_cam->updateDimensions(width(), height());

    delete m_img;
    m_img = new QImage(width(), height(), QImage::Format_RGB32);

    QElapsedTimer timer;
    timer.start();

    CameraPacket::deletePackets(m_packets);
    m_packets = CameraPacket::genPackets(m_img, m_tracer, *m_cam, m_sampleWidth);
    
    cout << "Time to resize image: " << timer.elapsed() << endl;
    timer.invalidate();

    m_refreshScreen = true;
    update();
}


void PaintCanvas::paintEvent(QPaintEvent* event) {
    (void) event;

    QElapsedTimer timer;

    if(m_refreshScreen) {
        timer.start();
    }
      
    if(m_piecesMoved || INTERP) {
        bool temp = m_refreshScreen;
        m_refreshScreen = false;
        
        while(m_tickCount > 0) {
            m_game->tick();
            --m_tickCount;
        }

        m_piecesMoved = false;

        for(auto it = m_primitives->begin(); it != m_primitives->end(); ++it) {
            delete *it;
        }

        m_primitives->clear();
        
        if(INTERP) {
            double fallAmount = 1.0 - m_gameTimer->remainingTime() / (double)m_gameTimer->interval();
            m_root->getPrimitives(m_primitives, m_game, fallAmount);

        } else {
            m_root->getPrimitives(m_primitives, m_game);
        }

        m_tracer->updatePrimitives(m_primitives);

        m_refreshScreen = temp;
    }

    if(m_refreshScreen) {
        m_refreshScreen = false;

        computeQImage();

        cout << "Time to compute image: " << timer.elapsed() << endl;
        timer.invalidate();
    }

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), *m_img);
}

void PaintCanvas::resizeEvent(QResizeEvent* event) {
    (void) event;

    m_resizeTimer->start();
}

void PaintCanvas::computeQImage() {
    m_index = 0;
    m_k = 1;

    pthread_mutex_init(&m_mutex, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if(m_printStatus) {
        cout << "0\% complete" << endl;
    }

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

    if(m_printStatus) {
        cout << "100\% complete" << endl;
    }

    pthread_mutex_destroy(&m_mutex);
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

        if(m_printStatus && index == (int) ((m_k/10.0) * numPackets)) {
            cout << m_k*10 << "\% complete" << endl;
            m_k++;
        }
    }       
}

void PaintCanvas::setTickSpeed(Speed speed)
{
    if(m_game != NULL) {
        switch(speed)
        {
            case Speed::slow:
                m_gameTimer->setInterval(500);
                break;
            case Speed::medium:
                m_gameTimer->setInterval(350);
                break;
            case Speed::fast:
                m_gameTimer->setInterval(200);
        }

        if(!m_paused) {
            m_gameTimer->start();
        }
    }
}

void PaintCanvas::setSlowSpeed() {
    setTickSpeed(Speed::slow);
}

void PaintCanvas::setMediumSpeed() {
    setTickSpeed(Speed::medium);
}

void PaintCanvas::setFastSpeed() {
    setTickSpeed(Speed::fast);
}

void PaintCanvas::moveLeft() {
    if(!(m_game == NULL || m_paused)) {
        m_game->moveLeft();
        m_piecesMoved = true;
        refresh();
    }
}

void PaintCanvas::moveRight() {
if(!(m_game == NULL || m_paused)) {
        m_game->moveRight();
        m_piecesMoved = true;
        refresh();
    }
}

void PaintCanvas::rotateCW() {
if(!(m_game == NULL || m_paused)) {
        m_game->rotateCW();
        m_piecesMoved = true;
        refresh();
    }
}

void PaintCanvas::rotateCCW() {
    if(!(m_game == NULL || m_paused)) {
        m_game->rotateCCW();
        m_piecesMoved = true;
        refresh();
    }
}

void PaintCanvas::drop() {
    if(!(m_game == NULL || m_paused)) {
        m_game->drop();
        m_piecesMoved = true;
        refresh();
    }
}


void PaintCanvas::newGame() {
    if(m_game != NULL) {
        m_game->reset();
        m_piecesMoved = true;

        update();
    }
}

void PaintCanvas::pause() {
    if(m_game != NULL) {
        if(m_paused) {
            m_gameTimer->start();
        } else {
            m_gameTimer->stop();
        }

        m_paused = !m_paused;
    }
}

void PaintCanvas::refresh() {
    if(m_piecesMoved || INTERP) {
        m_refreshScreen = true;
    }
    
    update();
}

void PaintCanvas::tick() {
    if(!m_game->isGameOver()) {
        ++m_tickCount;
        m_piecesMoved = true;
    }
}


