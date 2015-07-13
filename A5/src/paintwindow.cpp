// Spencer Van Leeuwen
// 20412199

#include <iostream>
#include <string>

#include "paintwindow.hpp"

using std::list;
using std::vector;
using std::string;

PaintWindow::PaintWindow(Camera* cam, const list<Light*>* lights, const Colour& ambient, 
        vector<Primitive*>* primitives, string filename)
{
    setWindowTitle("Ray Traced Tetris");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this, cam, lights, ambient, primitives, filename);

    layout->addWidget(m_canvas);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createMenu();
}

void PaintWindow::resizeCanvas() {
    m_canvas->resizeAction();
}

void PaintWindow::createMenu() {
    // Adding the drop down menu to the menubar
    m_menu_app = menuBar()->addMenu(tr("&Application"));

    // Adding the menu items for each drop down menu
    QAction* quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QList<QKeySequence>({QKeySequence::Quit, QKeySequence(Qt::Key_Q)}));
    addAction(quitAct);
    quitAct->setStatusTip(tr("Exits the program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    m_menu_app->addAction(quitAct);
}
