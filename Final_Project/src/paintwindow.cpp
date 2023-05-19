// Spencer Van Leeuwen
// 20412199

#include <iostream>
#include <string>

#include "paintwindow.hpp"

using std::list;
using std::vector;
using std::string;

PaintWindow::PaintWindow(Camera* cam, const list<Light*>* lights, const Colour& ambient, 
        SceneNode* root, string filename) 
{
    setWindowTitle("Ray Traced Tetris");

    QVBoxLayout *layout = new QVBoxLayout;

    m_canvas = new PaintCanvas(this, cam, lights, ambient, root, filename);
    m_game = m_canvas->m_game;

    layout->addWidget(m_canvas);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();
}

void PaintWindow::resizeCanvas() {
    m_canvas->resizeAction();
}

void PaintWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Escape: 
            QCoreApplication::instance()->quit();
            return;
        case Qt::Key_Left:
            m_canvas->moveLeft();
            break;
        case Qt::Key_Right:
            m_canvas->moveRight();
            break;
        case Qt::Key_Up:
            m_canvas->rotateCW();
            break;
        case Qt::Key_Down:
            m_canvas->rotateCCW();
            break;
        case Qt::Key_Space:
            m_canvas->drop();
            break;
        default: 
            QWidget::keyPressEvent(event);
            return;
    }
}

void PaintWindow::createActions() {
    createAppActions();
    createSpeedActions();
    createSampleActions();
    createAccelActions();
}

void PaintWindow::createAppActions()
{
    QAction* quitAct = new QAction(tr("&Quit"), this);
    QAction* newGameAct = new QAction(tr("&New Game"), this);
    QAction* pauseAct = new QAction(tr("&Pause"), this);
    QAction* statusAct = new QAction(tr("Print &Status"), this);
    QAction* saveAct = new QAction(tr("Sa&ve"), this);
    QAction* interpAct = new QAction(tr("&Keyframe Interpolation"), this);

    m_app_actions.push_back(quitAct);
    m_app_actions.push_back(newGameAct);
    m_app_actions.push_back(pauseAct);
    m_app_actions.push_back(statusAct);
    m_app_actions.push_back(saveAct);
    m_app_actions.push_back(interpAct);

    quitAct->setShortcuts(QList<QKeySequence>({QKeySequence::Quit, QKeySequence(Qt::Key_Q)}));
    newGameAct->setShortcut(Qt::Key_N);
    pauseAct->setShortcut(Qt::Key_P);
    statusAct->setShortcut(Qt::Key_S);
    saveAct->setShortcut(Qt::Key_V);
    interpAct->setShortcut(Qt::Key_I);

    quitAct->setStatusTip(tr("Exits the file"));
    newGameAct->setStatusTip(tr("Starts a new game"));
    pauseAct->setStatusTip(tr("Pauses the game"));
    statusAct->setStatusTip(tr("Prints the tracing status"));
    saveAct->setStatusTip(tr("Saves the image"));
    interpAct->setStatusTip(tr("Toggles keyframe interpolation"));

    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(pauseAct, SIGNAL(triggered()), this, SLOT(pause())); 
    connect(statusAct, SIGNAL(triggered()), this, SLOT(printStatus())); 
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save())); 
    connect(interpAct, SIGNAL(triggered()), this, SLOT(interp())); 

    statusAct->setCheckable(true);
    interpAct->setCheckable(true);

    for (auto& action : m_app_actions) {
        addAction(action);
    }
}

void PaintWindow::createSpeedActions()
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

void PaintWindow::createSampleActions() {
    m_group_samples = new QActionGroup(this);

    QAction* oneAct = new QAction(tr("&1 per pixel"), m_group_samples);
    QAction* twoAct = new QAction(tr("&4 per pixel"), m_group_samples);
    QAction* threeAct = new QAction(tr("&9 per pixel"), m_group_samples);
    QAction* fourAct = new QAction(tr("&16 per pixel"), m_group_samples);
    
    m_sample_actions.push_back(oneAct);
    m_sample_actions.push_back(twoAct);
    m_sample_actions.push_back(threeAct);
    m_sample_actions.push_back(fourAct);

    oneAct->setShortcut(Qt::Key_4);
    twoAct->setShortcut(Qt::Key_5);
    threeAct->setShortcut(Qt::Key_6);
    fourAct->setShortcut(Qt::Key_7);

    oneAct->setStatusTip(tr("Sets sample rate to 1 per pixel"));
    twoAct->setStatusTip(tr("Sets sample rate to 4 per pixel"));
    threeAct->setStatusTip(tr("Sets sample rate to 9 per pixel"));
    fourAct->setStatusTip(tr("Sets sample rate to 16 per pixel"));

    connect(oneAct, SIGNAL(triggered()), this, SLOT(setOneSamples()));
    connect(twoAct, SIGNAL(triggered()), this, SLOT(setTwoSamples()));
    connect(threeAct, SIGNAL(triggered()), this, SLOT(setThreeSamples()));
    connect(fourAct, SIGNAL(triggered()), this, SLOT(setFourSamples()));
    
    for (auto& action : m_sample_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    oneAct->setChecked(true);
}

void PaintWindow::createAccelActions()
{ 
    m_group_accel = new QActionGroup(this);

    QAction* noneAct = new QAction(tr("&None"), m_group_accel);
    QAction* bihAct = new QAction(tr("&BIH"), m_group_accel);
    QAction* allAct = new QAction(tr("&BIH and Packets"), m_group_accel);
    
    m_accel_actions.push_back(noneAct);
    m_accel_actions.push_back(bihAct);
    m_accel_actions.push_back(allAct);

    noneAct->setShortcut(Qt::Key_8);
    bihAct->setShortcut(Qt::Key_9);
    allAct->setShortcut(Qt::Key_0);

    noneAct->setStatusTip(tr("No acceleration"));
    bihAct->setStatusTip(tr("Use BIH"));
    allAct->setStatusTip(tr("Use BIH and ray packets"));

    connect(noneAct, SIGNAL(triggered()), this, SLOT(setNoAccel()));
    connect(bihAct, SIGNAL(triggered()), this, SLOT(setBihAccel()));
    connect(allAct, SIGNAL(triggered()), this, SLOT(setAllAccel()));
    
    for (auto& action : m_accel_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    allAct->setChecked(true);
}

void PaintWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_speed = menuBar()->addMenu(tr("&Speed"));
    m_menu_samples = menuBar()->addMenu(tr("&# of Samples"));
    m_menu_accel = menuBar()->addMenu(tr("A&cceleration"));

    for (auto& action : m_app_actions) {
        m_menu_app->addAction(action);
    }

    for (auto& action : m_speed_actions) {
        m_menu_speed->addAction(action);
    }
    
    for (auto& action : m_sample_actions) {
        m_menu_samples->addAction(action);
    }
    
    for (auto& action : m_accel_actions) {
        m_menu_accel->addAction(action);
    }
}

void PaintWindow::newGame() {
    m_canvas->newGame();
}

void PaintWindow::pause() {
    m_canvas->pause();
}

void PaintWindow::printStatus() {
    m_canvas->m_printStatus = !m_canvas->m_printStatus;
}

void PaintWindow::save() {
    m_canvas->saveImage();
}

void PaintWindow::setSlowSpeed() {
    m_canvas->setSlowSpeed();
}

void PaintWindow::setMediumSpeed() {
    m_canvas->setMediumSpeed();
}

void PaintWindow::setFastSpeed() {
    m_canvas->setFastSpeed();
}

void PaintWindow::setOneSamples() {
    m_canvas->setSampleWidth(1);
}

void PaintWindow::setTwoSamples() {
    m_canvas->setSampleWidth(2);
}

void PaintWindow::setThreeSamples() {
    m_canvas->setSampleWidth(3);
}

void PaintWindow::setFourSamples() {
    m_canvas->setSampleWidth(4);
}

void PaintWindow::setNoAccel() {
    BIH = false;
    PACKETS = false;
}

void PaintWindow::setBihAccel() {
    BIH = true;
    PACKETS = false;
}

void PaintWindow::setAllAccel() {
    BIH = true;
    PACKETS = true;
}

void PaintWindow::interp() {
    INTERP = !INTERP;
}
