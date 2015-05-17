#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include "Viewer.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void createActions();
    void createAppActions();
    void createDrawActions();
    void createSpeedActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_draw;
    QMenu* m_menu_speed;

    QActionGroup* m_group_draw;
    QActionGroup* m_group_speed;

    std::vector<QAction*> m_app_actions;
    std::vector<QAction*> m_draw_actions;
    std::vector<QAction*> m_speed_actions;
    
    Viewer* m_viewer;
    Game* m_game;
    QTimer* m_game_timer;

    enum Speed { slow, medium, fast };

    void setTickSpeed(Speed speed);

private slots:
    void setSlowSpeed();
    void setMediumSpeed();
    void setFastSpeed();

    void setWireframeMode();
    void setFaceMode();
    void setMulticolourMode();

    void newGame();
    void resetView();

    void tick();
};

#endif
