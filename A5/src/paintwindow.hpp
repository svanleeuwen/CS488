// Spencer Van Leeuwen
// 20412199

#ifndef PAINTWINDOW_HPP
#define PAINTWINDOW_HPP

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QtWidgets>
#include "paintcanvas.hpp"

#include <list>
#include <string>
#include <vector>

// class PaintCanvas;

class PaintWindow : public QMainWindow
{
    Q_OBJECT

public:
    PaintWindow(Camera* cam, const std::list<Light*>* lights, const Colour& ambient, 
            std::vector<Primitive*>* primitives, std::string filename);
    virtual ~PaintWindow() {}

    void resizeCanvas();

private:
    void createMenu();

    // Note: QMainWindow has its own QMenuBar where as QWidget does not
    // Each drop down menus
    QMenu* m_menu_app;
    
    // The canvas onto which the user draws.
    PaintCanvas* m_canvas;
};

#endif
