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
};

#endif
