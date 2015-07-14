#include "a4.hpp"
#include "camera.hpp"
#include "tracer.hpp"
#include "primitive.hpp"
#include "packet.hpp"
#include "paintwindow.hpp"

#include <math.h>
#include <iostream>
#include <pthread.h>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

#define NUMTHREADS 8

bool launch_qt(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // QImage size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
    int argc = 1;
    const char* name = "Ray Traced Tetris\0";
    QApplication app(argc, (char**)&name);

    Camera cam(width, height, eye, view, up, fov);

    vector<Primitive*>* primitives = new vector<Primitive*>();
    root->getPrimitives(primitives);
    delete root;
    
    PaintWindow window(&cam, &lights, ambient, primitives, filename);
    
    window.resize(window.sizeHint());
    int desktopArea = QApplication::desktop()->width() * 
                      QApplication::desktop()->height();

    int widgetArea = window.width() * window.height();

    if (((float)widgetArea / (float) desktopArea) < 0.75f) {
        window.show();
    } else {
        window.showMaximized();
    }

    return app.exec();
}
