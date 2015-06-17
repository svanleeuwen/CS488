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

private:
    void createActions();
    void createAppActions();
    void createModeActions();
    void createEditActions();
    void createOptionsActions();
    
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
    QMenu* m_menu_mode;
    QMenu* m_menu_edit;
    QMenu* m_menu_options;

    QActionGroup* m_group_mode;

    std::vector<QAction*> m_app_actions;
    std::vector<QAction*> m_mode_actions;
    std::vector<QAction*> m_edit_actions;
    std::vector<QAction*> m_options_actions;

    Viewer* m_viewer;

private slots:
    void resetPos();
    void resetOri();
    void resetJoints();
    void resetAll();

    void setPosOri();
    void setJoints();

    void undo();
    void redo();

    void toggleCircle();
    void toggleZBuffer();
    void toggleBackface();
    void toggleFrontface();
};

#endif
