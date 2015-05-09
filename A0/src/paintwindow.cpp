#include <QtWidgets>
#include <iostream>
#include "paintwindow.hpp"

PaintWindow::PaintWindow() {
    setWindowTitle("488 Paint");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this);
    layout->addWidget(m_canvas);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createMenu();
}

void PaintWindow::createMenu() {
    // Adding the drop down menu to the menubar
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_tools = menuBar()->addMenu(tr("&Tools"));
    m_menu_help = menuBar()->addMenu(tr("&Help"));

    // Adding the menu items for each drop down menu
    QAction* quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Exits the program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    m_menu_app->addAction(quitAct);

    QAction* drawLineAct = new QAction(tr("&Line"), this);
    drawLineAct->setStatusTip(tr("Draws a line"));
    connect(drawLineAct, SIGNAL(triggered()), this, SLOT(set_line()));

    QAction* drawOvalAct = new QAction(tr("&Oval"), this);
    drawOvalAct->setStatusTip(tr("Draws an Oval"));
    connect(drawOvalAct, SIGNAL(triggered()), this, SLOT(set_oval()));

    QAction* drawRectangleAct = new QAction(tr("&Rectangle"), this);
    drawRectangleAct->setStatusTip(tr("Draws a rectangle"));
    connect(drawRectangleAct, SIGNAL(triggered()), this, SLOT(set_rect()));

    m_menu_tools->addAction(drawLineAct);
    m_menu_tools->addAction(drawOvalAct);
    m_menu_tools->addAction(drawRectangleAct);

    QAction* helpLineAct = new QAction(tr("&Line Help"), this);
    helpLineAct->setStatusTip(tr("Help Instructions"));
    connect(helpLineAct, SIGNAL(triggered()), this, SLOT(help_line()));
    m_menu_help->addAction(helpLineAct);
}

void PaintWindow::help_line() {
    const char* message =
    "Drawing a Line\n"
    "\n"
    "To draw a line, press the left mouse button to mark the beginning of the line.  Drag the mouse to the end of the line and release the button.";

    QMessageBox msgBox;
    msgBox.setText(QString(message));
    msgBox.exec();
}

void PaintWindow::set_line() {
    m_canvas->set_mode(PaintCanvas::DRAW_LINE);
}

void PaintWindow::set_oval() {
    m_canvas->set_mode(PaintCanvas::DRAW_OVAL);
}

void PaintWindow::set_rect() {
    m_canvas->set_mode(PaintCanvas::DRAW_RECTANGLE);
}
