/*
 * Name: Spencer Van Leeuwen
 * Student Number: 20412199
 * User-id: srvanlee
 */
#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Tetrominoes on the Wall");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    m_game = new Game();

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, m_game, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
    m_viewer->show();

    m_game_timer = new QTimer(this);
    connect(m_game_timer, SIGNAL(timeout()), this, SLOT(tick()));
    setTickSpeed(Speed::slow);

    setFaceMode();

    createActions();
    createMenu();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape: 
            QCoreApplication::instance()->quit();
            return;
        case Qt::Key_Left:
            m_game->moveLeft();
            break;
        case Qt::Key_Right:
            m_game->moveRight();
            break;
        case Qt::Key_Up:
            m_game->rotateCCW();
            break;
        case Qt::Key_Down:
            m_game->rotateCW();
            break;
        case Qt::Key_Space:
            m_game->drop();
            break;
        default: 
            QWidget::keyPressEvent(event);
            return;
    }

    m_viewer->update();
}

void AppWindow::createActions() {
    createAppActions();
    createDrawActions();
    createSpeedActions();
}

void AppWindow::createAppActions()
{
    QAction* quitAct = new QAction(tr("&Quit"), this);
    QAction* newGameAct = new QAction(tr("&New Game"), this);
    QAction* resetAct = new QAction(tr("&Reset"), this);

    m_app_actions.push_back(quitAct);
    m_app_actions.push_back(newGameAct);
    m_app_actions.push_back(resetAct);

    quitAct->setShortcuts(QList<QKeySequence>({QKeySequence::Quit, QKeySequence(Qt::Key_Q)}));
    newGameAct->setShortcut(Qt::Key_N);
    resetAct->setShortcut(Qt::Key_R);

    quitAct->setStatusTip(tr("Exits the file"));
    newGameAct->setStatusTip(tr("Starts a new game"));
    resetAct->setStatusTip(tr("Resets the view of the game"));

    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(resetAct, SIGNAL(triggered()), this, SLOT(resetView())); 

    for (auto& action : m_app_actions) {
        addAction(action);
    }
}

void AppWindow::createDrawActions()
{
    m_group_draw = new QActionGroup(this);

    QAction* wireFrameAct = new QAction(tr("&Wire-frame"), m_group_draw);
    QAction* faceAct = new QAction(tr("&Face"), m_group_draw);
    QAction* multiAct = new QAction(tr("&Multicoloured"), m_group_draw);

    m_draw_actions.push_back(wireFrameAct);
    m_draw_actions.push_back(faceAct);
    m_draw_actions.push_back(multiAct);

    wireFrameAct->setShortcut(Qt::Key_W);
    faceAct->setShortcut(Qt::Key_F);
    multiAct->setShortcut(Qt::Key_M);

    wireFrameAct->setStatusTip(tr("Draws game in wire-frame mode"));
    faceAct->setStatusTip(tr("Draws game with the faces of shapes coloured"));
    multiAct->setStatusTip(tr("Draws the game with each cube face a different colour"));

    connect(wireFrameAct, SIGNAL(triggered()), this, SLOT(setWireframeMode()));
    connect(faceAct, SIGNAL(triggered()), this, SLOT(setFaceMode()));
    connect(multiAct, SIGNAL(triggered()), this, SLOT(setMulticolourMode()));

    for (auto& action : m_draw_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    faceAct->setChecked(true);
}

void AppWindow::createSpeedActions()
{ 
    m_group_speed = new QActionGroup(this);

    QAction* slowAct = new QAction(tr("&Slow"), m_group_speed);
    QAction* mediumAct = new QAction(tr("&Medium"), m_group_speed);
    QAction* fastAct = new QAction(tr("&Fast"), m_group_speed);
    
    m_speed_actions.push_back(slowAct);
    m_speed_actions.push_back(mediumAct);
    m_speed_actions.push_back(fastAct);

    slowAct->setShortcut(Qt::Key_1);
    mediumAct->setShortcut(Qt::Key_2);
    fastAct->setShortcut(Qt::Key_3);

    slowAct->setStatusTip(tr("Sets fall rate to slow"));
    mediumAct->setStatusTip(tr("Sets fall rate to medium"));
    fastAct->setStatusTip(tr("Sets fall rate to fast"));

    connect(slowAct, SIGNAL(triggered()), this, SLOT(setSlowSpeed()));
    connect(mediumAct, SIGNAL(triggered()), this, SLOT(setMediumSpeed()));
    connect(fastAct, SIGNAL(triggered()), this, SLOT(setFastSpeed()));
    
    for (auto& action : m_speed_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    slowAct->setChecked(true);
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_draw = menuBar()->addMenu(tr("&Draw Mode"));
    m_menu_speed = menuBar()->addMenu(tr("&Speed"));

    for (auto& action : m_app_actions) {
        m_menu_app->addAction(action);
    }

    for (auto& action : m_draw_actions) {
        m_menu_draw->addAction(action);
    }

    for (auto& action : m_speed_actions) {
        m_menu_speed->addAction(action);
    }
}

void AppWindow::setTickSpeed(Speed speed)
{
    switch(speed)
    {
        case Speed::slow:
            m_game_timer->setInterval(500);
            break;
        case Speed::medium:
            m_game_timer->setInterval(300);
            break;
        case Speed::fast:
            m_game_timer->setInterval(100);
    }

    m_game_timer->start();
}

void AppWindow::setSlowSpeed() {
    setTickSpeed(Speed::slow);
}

void AppWindow::setMediumSpeed() {
    setTickSpeed(Speed::medium);
}

void AppWindow::setFastSpeed() {
    setTickSpeed(Speed::fast);
}

void AppWindow::setWireframeMode() {
    m_viewer->setDrawMode(Viewer::DrawMode::wireframe);
}

void AppWindow::setFaceMode() {
    m_viewer->setDrawMode(Viewer::DrawMode::face);
}

void AppWindow::setMulticolourMode() {
    m_viewer->setDrawMode(Viewer::DrawMode::multicolour);
}

void AppWindow::newGame() {
    m_game->reset();
}

void AppWindow::resetView() {
    m_viewer->resetView();
}

void AppWindow::tick() {
    m_game->tick();
}
