#include "a4.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "tracer.hpp"
#include <math.h>

#include <iostream>

using std::cout;
using std::endl;

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
    Camera cam(width, height, eye, view, up, fov);

    Ray* rays = new Ray[height*width];

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            rays[y*height + x] = cam.getRay(x, y);
        }
    }

    Tracer tracer(root, cam, ambient, lights);

    Image img(width, height, 3);
    int i = 1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if(y*height + x == (int) ((i/10.0) * (width*height))) {
                cout << i*10 << "\% complete" << endl;
                i++;
            }

            Colour colour = Colour(0.0, 0.0, 0.0);
            bool hit = tracer.traceRay(rays[y*height + x], colour);

            if(hit) {
                img(x, y, 0) = colour.R();
                img(x, y, 1) = colour.G();
                img(x, y, 2) = colour.B();
            } else {
                img(x, y, 0) = ((double)x)/(double)width;
                img(x, y, 1) = ((double)y)/(double)height;
                img(x, y, 2) = 1.0 - ((double)x)/(double)width;
            }
        }
    }

    cout << "100\% complete" << endl;

    img.savePng(filename);
}
