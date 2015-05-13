// Spencer Van Leeuwen
// 20412199

#include <iostream>
#include "paintwindow.hpp"

PaintWindow::PaintWindow() {
    setWindowTitle("488 Paint");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this);
    m_button = new QPushButton("Quit");
    connect(m_button, SIGNAL(clicked()), this, SLOT(close()));

    layout->addWidget(m_canvas);
    layout->addWidget(m_button);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createMenu();
}



void PaintWindow::createMenu() {
    // Adding the drop down menu to the menubar
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_tools = menuBar()->addMenu(tr("&Tools"));
    m_menu_colour = menuBar()->addMenu(tr("&Colour"));
    m_menu_help = menuBar()->addMenu(tr("&Help"));

    // Adding tools group
    m_group_tools = new QActionGroup(this);
    m_group_colour = new QActionGroup(this);

    // Adding the menu items for each drop down menu
    QAction* quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    addAction(quitAct);
    quitAct->setStatusTip(tr("Exits the program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    m_menu_app->addAction(quitAct);

    QAction* clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(Qt::Key_C);
    addAction(clearAct);
    clearAct->setStatusTip(tr("Clears the canvas"));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));
    m_menu_app->addAction(clearAct);

    QAction* fillBlackAct = new QAction(tr("Bl&ack"), m_group_colour);
    fillBlackAct->setStatusTip(tr("Sets fill colour to black"));
    connect(fillBlackAct, SIGNAL(triggered()), this, SLOT(set_black()));
    fillBlackAct->setCheckable(true);
    fillBlackAct->setChecked(true);

    QAction* fillRedAct = new QAction(tr("&Red"), m_group_colour);
    fillRedAct->setStatusTip(tr("Sets fill colour to red"));
    connect(fillRedAct, SIGNAL(triggered()), this, SLOT(set_red()));
    fillRedAct->setCheckable(true);

    QAction* fillGreenAct = new QAction(tr("&Green"), m_group_colour);
    fillGreenAct->setStatusTip(tr("Sets fill colour to green"));
    connect(fillGreenAct, SIGNAL(triggered()), this, SLOT(set_green()));
    fillGreenAct->setCheckable(true);

    QAction* fillBlueAct = new QAction(tr("Bl&ue"), m_group_colour);
    fillBlueAct->setStatusTip(tr("Sets fill colour to blue"));
    connect(fillBlueAct, SIGNAL(triggered()), this, SLOT(set_blue()));
    fillBlueAct->setCheckable(true);

    QAction* drawLineAct = new QAction(tr("&Line"), m_group_tools);
    drawLineAct->setShortcut(Qt::Key_L);
    addAction(drawLineAct);
    drawLineAct->setStatusTip(tr("Draws a line"));
    drawLineAct->setCheckable(true);
    connect(drawLineAct, SIGNAL(triggered()), this, SLOT(set_line()));

    QAction* drawOvalAct = new QAction(tr("&Oval"), m_group_tools);
    drawOvalAct->setShortcut(Qt::Key_O);
    addAction(drawOvalAct);
    drawOvalAct->setStatusTip(tr("Draws an Oval"));
    drawOvalAct->setCheckable(true);
    connect(drawOvalAct, SIGNAL(triggered()), this, SLOT(set_oval()));

    QAction* drawRectangleAct = new QAction(tr("&Rectangle"), m_group_tools);
    drawRectangleAct->setShortcut(Qt::Key_R);
    addAction(drawRectangleAct);
    drawRectangleAct->setStatusTip(tr("Draws a rectangle"));
    drawRectangleAct->setCheckable(true);
    connect(drawRectangleAct, SIGNAL(triggered()), this, SLOT(set_rect()));

    drawLineAct->setChecked(true);
    m_menu_tools->addAction(drawLineAct);
    m_menu_tools->addAction(drawOvalAct);
    m_menu_tools->addAction(drawRectangleAct);

    m_menu_colour->addAction(fillBlackAct);
    m_menu_colour->addAction(fillRedAct);
    m_menu_colour->addAction(fillGreenAct);
    m_menu_colour->addAction(fillBlueAct);

    QAction* helpLineAct = new QAction(tr("&Line Help"), this);
    helpLineAct->setStatusTip(tr("Help Instructions"));
    connect(helpLineAct, SIGNAL(triggered()), this, SLOT(help_line()));
    m_menu_help->addAction(helpLineAct);

    QAction* helpOvalAct = new QAction(tr("&Oval Help"), this);
    helpOvalAct->setStatusTip(tr("Help Instructions"));
    connect(helpOvalAct, SIGNAL(triggered()), this, SLOT(help_oval()));
    m_menu_help->addAction(helpOvalAct);

    QAction* helpRectangleAct = new QAction(tr("&Rectangle Help"), this);
    helpRectangleAct->setStatusTip(tr("Help Instructions"));
    connect(helpRectangleAct, SIGNAL(triggered()), this, SLOT(help_rectangle()));
    m_menu_help->addAction(helpRectangleAct);
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

void PaintWindow::help_oval() {
    const char* message =
    "Drawing an Oval\n"
    "\n"
    "To draw an oval, press the left mouse button to mark the beginning of the oval.  Drag the mouse to the end of the oval and release the button.";

    QMessageBox msgBox;
    msgBox.setText(QString(message));
    msgBox.exec();
}

void PaintWindow::help_rectangle() {
    const char* message =
    "Drawing a Rectangle\n"
    "\n"
    "To draw a rectangle, press the left mouse button to mark the beginning of the rectangle.  Drag the mouse to the end of the rectangle and release the button.";

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

void PaintWindow::set_black() {
    m_canvas->set_fill(QColor(0,0,0));
}

void PaintWindow::set_red() {
    m_canvas->set_fill(QColor(255,0,0));
}

void PaintWindow::set_green() {
    m_canvas->set_fill(QColor(0,255,0));
}

void PaintWindow::set_blue() {
    m_canvas->set_fill(QColor(0,0,255));
}

void PaintWindow::clear() {
    m_canvas->clear();
}
