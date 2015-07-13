// Spencer Van Leeuwen
// 20412199

#ifndef PAINTCANVAS_HPP
#define PAINTCANVAS_HPP

#include <list>
#include <vector>
#include <QWidget>
#include <QPainter>
#include <QString>

#include "algebra.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "tracer.hpp"
#include "packet.hpp"

class PaintCanvas : public QWidget {

    Q_OBJECT

public:
    PaintCanvas(QWidget *parent, Camera* cam, const std::list<Light*>* lights, Colour ambient, 
        std::vector<Primitive*>* primitives, std::string& filename);
    virtual ~PaintCanvas();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void saveImage();

public slots:
    void resizeAction();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

    Camera* m_initCam;
    Camera* m_cam;
    
    Tracer* m_tracer;
    QImage* m_img;

    const std::list<Light*>* m_lights;
    Colour m_ambient;

    std::vector<Primitive*>* m_primitives;
    QString m_filename;

private:
    void computeQImage();

    static void* thread_bootstrap(void* canvas);
    void computePixels();
    
    int m_index;
    int m_k;

    std::vector<Packet*>* m_packets;

    static const int NUMTHREADS = 8;
    pthread_t m_threads[NUMTHREADS];
    pthread_mutex_t m_mutex;

    QTimer* m_resizeTimer;

    bool m_newFrame;
};

#endif
