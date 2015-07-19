// Spencer Van Leeuwen
// 20412199

#ifndef PAINTWINDOW_HPP
#define PAINTWINDOW_HPP

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QtWidgets>

#include <list>
#include <string>
#include <vector>

#include "paintcanvas.hpp"
#include "scene.hpp"
#include "game.hpp"

class PaintWindow : public QMainWindow
{
    Q_OBJECT

public:
    PaintWindow(Camera* cam, const std::list<Light*>* lights, const Colour& ambient, 
            SceneNode* root, std::string filename);
    virtual ~PaintWindow() {}

    void resizeCanvas();

private:
    virtual void keyPressEvent(QKeyEvent* event);

    void createActions();
    void createMenu();
    
    void createAppActions();
    void createSpeedActions();
    void createSampleActions();
    void createAccelActions();

    PaintCanvas* m_canvas;

    QMenu* m_menu_app;
    QMenu* m_menu_speed;
    QMenu* m_menu_samples;
    QMenu* m_menu_accel;

    QActionGroup* m_group_speed;
    QActionGroup* m_group_samples;
    QActionGroup* m_group_accel;

    std::vector<QAction*> m_app_actions;
    std::vector<QAction*> m_speed_actions;
    std::vector<QAction*> m_sample_actions;
    std::vector<QAction*> m_accel_actions;

    Game* m_game;

private slots:
    void newGame(); 
    void pause();
    void printStatus();
    void save();
    
    void setSlowSpeed();
    void setMediumSpeed();
    void setFastSpeed();

    void setOneSamples();
    void setTwoSamples();
    void setThreeSamples();
    void setFourSamples();

    void setNoAccel();
    void setBihAccel();
    void setAllAccel();
};

#endif
