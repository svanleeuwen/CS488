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
#include "game.hpp"
#include "scene.hpp"

class PaintCanvas : public QWidget {

    Q_OBJECT

public:
    PaintCanvas(QWidget *parent, Camera* cam, const std::list<Light*>* lights, Colour ambient, 
        SceneNode* root, std::string& filename);
    virtual ~PaintCanvas();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;


    enum Speed { slow, medium, fast };

    void saveImage();
    void setTickSpeed(Speed speed);
    void setSampleWidth(int width);

    void newGame();
    void pause();
    
    void setSlowSpeed();
    void setMediumSpeed();
    void setFastSpeed();

    void moveLeft();
    void moveRight();
    void rotateCW();
    void rotateCCW();
    void drop();

    Game* m_game;
    bool m_printStatus;

public slots:
    void resizeAction();
    void refresh();

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

    SceneNode* m_root;

private:
    void computeQImage();

    static void* thread_bootstrap(void* canvas);
    void computePixels();

    int m_index;
    int m_k;

    std::vector<CameraPacket*>* m_packets;

    static const int NUMTHREADS = 8;
    pthread_t m_threads[NUMTHREADS];
    pthread_mutex_t m_mutex;

    QTimer* m_resizeTimer;
    QTimer* m_gameTimer;
    QTimer* m_updateTimer;

    bool m_refreshScreen;
    bool m_paused;
    bool m_piecesMoved;

    int m_tickCount;
    int m_sampleWidth;

private slots:
    void tick();
};
#endif
