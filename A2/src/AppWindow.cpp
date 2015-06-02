#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Assignment Two");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);

    m_mode_text = new QLabel(this);
    m_mode_text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(m_mode_text);

    m_viewer->mNearText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(m_viewer->mNearText);
    
    m_viewer->mFarText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(m_viewer->mFarText);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createActions();
    createMenu();

    setRotateModel();
}

void AppWindow::createActions() {
    createAppActions();

    m_group_mode = new QActionGroup(this);

    createViewActions();
    createModelActions();

    viewportAct = new QAction(tr("&Viewport"), m_group_mode);
    viewportAct->setShortcut(Qt::Key_V);
    viewportAct->setStatusTip(tr("Enables viewport mode"));
    connect(viewportAct, SIGNAL(triggered()), this, SLOT(setViewport()));
    addAction(viewportAct);
    viewportAct->setCheckable(true);
}

void AppWindow::createAppActions() {
    QAction* quitAct = new QAction(tr("&Quit"), this);
    QAction* resetAct = new QAction(tr("&Reset"), this);

    m_app_actions.push_back(quitAct);
    m_app_actions.push_back(resetAct);

    quitAct->setShortcuts(QList<QKeySequence>({QKeySequence::Quit, QKeySequence(Qt::Key_Q)}));
    resetAct->setShortcut(Qt::Key_A);
    
    quitAct->setStatusTip(tr("Exits the file"));
    resetAct->setStatusTip(tr("Resets the view and model"));

    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(resetAct, SIGNAL(triggered()), this, SLOT(reset()));

    for (auto& action: m_app_actions) {
        addAction(action);
    }
}

void AppWindow::createViewActions() {
    QAction* vRotateAct  = new QAction(tr("&Rotate View"), m_group_mode);
    QAction* vTranslateAct = new QAction(tr("&Translate View"), m_group_mode);
    QAction* vPerspectiveAct = new QAction(tr("&Perspective"), m_group_mode);

    m_view_actions.push_back(vRotateAct);
    m_view_actions.push_back(vTranslateAct);
    m_view_actions.push_back(vPerspectiveAct);

    vRotateAct->setShortcut(Qt::Key_O);
    vTranslateAct->setShortcut(Qt::Key_N);
    vPerspectiveAct->setShortcut(Qt::Key_P);

    vRotateAct->setStatusTip(tr("Rotate eyepoint"));
    vTranslateAct->setStatusTip(tr("Translate eyepoint"));
    vPerspectiveAct->setStatusTip(tr("Change perspective"));

    connect(vRotateAct, SIGNAL(triggered()), this, SLOT(setRotateView()));
    connect(vTranslateAct, SIGNAL(triggered()), this, SLOT(setTransView()));
    connect(vPerspectiveAct, SIGNAL(triggered()), this, SLOT(setPerspective()));

    for (auto& action: m_view_actions) {
        addAction(action);
        action->setCheckable(true);
    }
}

void AppWindow::createModelActions() {
    QAction* mRotateAct  = new QAction(tr("&Rotate Model"), m_group_mode);
    QAction* mTranslateAct = new QAction(tr("&Translate Model"), m_group_mode);
    QAction* mScaleAct = new QAction(tr("&Scale Model"), m_group_mode);

    m_model_actions.push_back(mRotateAct);
    m_model_actions.push_back(mTranslateAct);
    m_model_actions.push_back(mScaleAct);

    mRotateAct->setShortcut(Qt::Key_R);
    mTranslateAct->setShortcut(Qt::Key_T);
    mScaleAct->setShortcut(Qt::Key_S);

    mRotateAct->setStatusTip(tr("Rotate model"));
    mTranslateAct->setStatusTip(tr("Translate model"));
    mScaleAct->setStatusTip(tr("Scale Model"));

    connect(mRotateAct, SIGNAL(triggered()), this, SLOT(setRotateModel()));
    connect(mTranslateAct, SIGNAL(triggered()), this, SLOT(setTransModel()));
    connect(mScaleAct, SIGNAL(triggered()), this, SLOT(setScaleModel()));

    for (auto& action: m_model_actions) {
        addAction(action);
        action->setCheckable(true);
    }

    mRotateAct->setChecked(true);
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
    m_menu_mode = menuBar()->addMenu(tr("&Mode"));

    for (auto& action : m_app_actions) {
        m_menu_app->addAction(action);
    }
    
    for (auto& action : m_view_actions) {
        m_menu_mode->addAction(action);
    }

    m_menu_mode->addSeparator();

    for (auto& action : m_model_actions) {
        m_menu_mode->addAction(action);
    }

    m_menu_mode->addSeparator();

    m_menu_mode->addAction(viewportAct);
}

void AppWindow::setRotateView() {
    m_viewer->set_mode(Viewer::Mode::rotateView);
    m_mode_text->setText(QString("Mode: Rotate View"));
}

void AppWindow::setTransView() {
    m_viewer->set_mode(Viewer::Mode::transView);
    m_mode_text->setText(QString("Mode: Translate View"));
}

void AppWindow::setPerspective() {
    m_viewer->set_mode(Viewer::Mode::perspective);
    m_mode_text->setText(QString("Mode: Perspective"));
}

void AppWindow::setRotateModel() {
    m_viewer->set_mode(Viewer::Mode::rotateModel);
    m_mode_text->setText(QString("Mode: Rotate Model"));
}

void AppWindow::setTransModel() {
    m_viewer->set_mode(Viewer::Mode::transModel);
    m_mode_text->setText(QString("Mode: Translate Model"));
}

void AppWindow::setScaleModel() {
    m_viewer->set_mode(Viewer::Mode::scaleModel);
    m_mode_text->setText(QString("Mode: Scale Model"));
}

void AppWindow::setViewport() {
    m_viewer->set_mode(Viewer::Mode::viewport);
    m_mode_text->setText(QString("Mode: Viewport"));
}

void AppWindow::reset() {
    m_viewer->reset_view();
    m_mode_text->setText(QString("Mode: Rotate Model"));

}
