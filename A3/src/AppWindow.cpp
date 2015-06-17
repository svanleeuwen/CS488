#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Assignment Three");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();
}

void AppWindow::createActions() {
    createAppActions();
    createModeActions();
    createEditActions();
    createOptionsActions();
}

void AppWindow::createAppActions() {
    QAction* quitAct = new QAction(tr("&Quit"), this);
    QAction* rPosAct = new QAction(tr("Reset &Position"), this);
    QAction* rOriAct = new QAction(tr("Reset &Orientation"), this);
    QAction* rJointAct = new QAction(tr("Reset &Joints"), this);
    QAction* rAllAct = new QAction(tr("Reset &All"), this);
    
    m_app_actions.push_back(quitAct);
    m_app_actions.push_back(rPosAct);
    m_app_actions.push_back(rOriAct);
    m_app_actions.push_back(rJointAct);
    m_app_actions.push_back(rAllAct);

    quitAct->setShortcuts(QList<QKeySequence>({QKeySequence::Quit, QKeySequence(Qt::Key_Q)}));
    rPosAct->setShortcut(Qt::Key_I);
    rOriAct->setShortcut(Qt::Key_O);
    rJointAct->setShortcut(Qt::Key_N);
    rAllAct->setShortcut(Qt::Key_A);

    quitAct->setStatusTip(tr("Exits the file"));
    rPosAct->setStatusTip(tr("Resets the position of the puppet"));
    rOriAct->setStatusTip(tr("Resets the orientation of the puppet"));
    rJointAct->setStatusTip(tr("Resets joint angles of the puppet"));
    rAllAct->setStatusTip(tr("Resets the puppet"));

    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(rPosAct, SIGNAL(triggered()), this, SLOT(resetPos()));
    connect(rOriAct, SIGNAL(triggered()), this, SLOT(resetOri()));
    connect(rJointAct, SIGNAL(triggered()), this, SLOT(resetJoints()));
    connect(rAllAct, SIGNAL(triggered()), this, SLOT(resetAll()));

    for (auto& action: m_app_actions) {
        addAction(action);
    }
}

void AppWindow::createModeActions() {
    m_group_mode = new QActionGroup(this);

    QAction* mPosOriAct = new QAction(tr("&Position/Orientation"), m_group_mode);
    QAction* mJointsAct = new QAction(tr("&Joints"), m_group_mode);

    m_mode_actions.push_back(mPosOriAct);
    m_mode_actions.push_back(mJointsAct);

    mPosOriAct->setShortcut(Qt::Key_P);
    mJointsAct->setShortcut(Qt::Key_J);

    mPosOriAct->setStatusTip(tr("Translate and rotate the puppet"));
    mJointsAct->setStatusTip(tr("Control joint angles"));

    connect(mPosOriAct, SIGNAL(triggered()), this, SLOT(setPosOri()));
    connect(mJointsAct, SIGNAL(triggered()), this, SLOT(setJoints()));

    for (auto& action: m_mode_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    mPosOriAct->setChecked(true);
}

void AppWindow::createEditActions() {
    QAction* undoAct = new QAction(tr("&Undo"), this);
    QAction* redoAct = new QAction(tr("&Redo"), this);

    m_edit_actions.push_back(undoAct);
    m_edit_actions.push_back(redoAct);

    undoAct->setShortcut(Qt::Key_U);
    redoAct->setShortcut(Qt::Key_R);

    undoAct->setStatusTip(tr("Undo the previous transformation"));
    redoAct->setStatusTip(tr("Redo the next transformation"));

    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    for (auto& action: m_edit_actions) {
        addAction(action);
    }
}

void AppWindow::createOptionsActions() {
    QAction* circleAct = new QAction(tr("&Circle"), this);
    QAction* zBufferAct = new QAction(tr("&Z-Buffer"), this);
    QAction* backfaceAct = new QAction(tr("&Backface Cull"), this);
    QAction* frontfaceAct = new QAction(tr("&Frontface Cull"), this);

    m_options_actions.push_back(circleAct);
    m_options_actions.push_back(zBufferAct);
    m_options_actions.push_back(backfaceAct);
    m_options_actions.push_back(frontfaceAct);

    circleAct->setShortcut(Qt::Key_C);
    zBufferAct->setShortcut(Qt::Key_Z);
    backfaceAct->setShortcut(Qt::Key_B);
    frontfaceAct->setShortcut(Qt::Key_F);

    circleAct->setStatusTip(tr("Toggles the circle for the trackball"));
    zBufferAct->setStatusTip(tr("Toggles the z-buffer"));
    backfaceAct->setStatusTip(tr("Toggles backface culling"));
    frontfaceAct->setStatusTip(tr("Toggles frontface culling"));
    
    connect(circleAct, SIGNAL(triggered()), this, SLOT(toggleCircle()));
    connect(zBufferAct, SIGNAL(triggered()), this, SLOT(toggleZBuffer()));
    connect(backfaceAct, SIGNAL(triggered()), this, SLOT(toggleBackface()));
    connect(frontfaceAct, SIGNAL(triggered()), this, SLOT(toggleFrontface()));

    for (auto& action: m_options_actions) {
        addAction(action);
        action->setCheckable(true);
        action->setChecked(false);
    }
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_mode = menuBar()->addMenu(tr("&Mode"));
    m_menu_edit = menuBar()->addMenu(tr("&Edit"));
    m_menu_options = menuBar()->addMenu(tr("&Options"));

    for (auto& action : m_app_actions) {
        m_menu_app->addAction(action);
    }
    
    for (auto& action : m_mode_actions) {
        m_menu_mode->addAction(action);
    }
   
    for (auto& action : m_edit_actions) {
        m_menu_edit->addAction(action);
    }
  
    for (auto& action : m_options_actions) {
        m_menu_options->addAction(action);
    }
}

void AppWindow::resetPos() {
    m_viewer->resetPos();
}

void AppWindow::resetOri() {
    m_viewer->resetOri();
}

void AppWindow::resetJoints() {
    m_viewer->resetJoints();
}

void AppWindow::resetAll() {
    m_viewer->resetPos();
    m_viewer->resetOri();
    m_viewer->resetJoints();
}

void AppWindow::setPosOri() {
    m_viewer->mPosOriMode = true;
}

void AppWindow::setJoints() {
    m_viewer->mPosOriMode = false; 
}

void AppWindow::undo() {
    m_viewer->undo();
}

void AppWindow::redo() {
    m_viewer->redo();
}

void AppWindow::toggleCircle() {
    m_viewer->toggleTrackingCircle();
}

void AppWindow::toggleZBuffer() {
    m_viewer->toggleZBuffer();
}

void AppWindow::toggleBackface() {
    m_viewer->toggleBackface();
}

void AppWindow::toggleFrontface() {
    m_viewer->toggleFrontface();
}
