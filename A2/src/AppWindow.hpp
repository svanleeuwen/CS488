#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include <QLabel>
#include "Viewer.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();

private:
    void createActions();
    void createAppActions();
    void createViewActions();
    void createModelActions();

    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_mode;

    QActionGroup* m_group_mode;
    QAction* m_default_action;

    std::vector<QAction*> m_app_actions;
    std::vector<QAction*> m_view_actions;
    std::vector<QAction*> m_model_actions;
    QAction* viewportAct;

    Viewer* m_viewer;

    QLabel* m_mode_text;

private slots:
    void setRotateView();
    void setTransView();
    void setPerspective();

    void setRotateModel();
    void setTransModel();
    void setScaleModel();

    void setViewport();

    void reset();
};

#endif
