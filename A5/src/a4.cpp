#include "a4.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "tracer.hpp"
#include <math.h>

#include <iostream>
#include <pthread.h>

using std::cout;
using std::endl;

#define NUMTHREADS 8

namespace {
    int i;
    int j;
    int k;

    struct thread_data{
        Image* img;
        Ray* rays;
        Tracer* tracer;
        int width;
        int height;
    };

    struct thread_data mydata;

    pthread_t threads[NUMTHREADS];
    pthread_mutex_t mutex;
}

void* computeImage(void* arg) {
    (void)arg;

    int x;
    int y;

    Ray* rays = mydata.rays;
    Image* img = mydata.img;
    Tracer* tracer = mydata.tracer;
    int width = mydata.width;
    int height = mydata.height;

    while(true) {
        pthread_mutex_lock(&mutex);
        if(j >= height) {
            pthread_mutex_unlock(&mutex);
            pthread_exit((void*) 0);
        }

        x = i;
        y = j;

        if(++i >= width) {
            i = 0;
            j++;
        }
        pthread_mutex_unlock(&mutex);

        if(y*height + x == (int) ((k/10.0) * (width*height))) {
            cout << k*10 << "\% complete" << endl;
            k++;
        }
       
        Colour colour = Colour(0.0, 0.0, 0.0);
        bool hit = tracer->traceRay(rays[y*height + x], colour);

        if(hit) {
            (*img)(x, y, 0) = colour.R();
            (*img)(x, y, 1) = colour.G();
            (*img)(x, y, 2) = colour.B();
        } else {
            (*img)(x, y, 0) = ((double)x)/(double)width;
            (*img)(x, y, 1) = ((double)y)/(double)height;
            (*img)(x, y, 2) = 1.0 - ((double)x)/(double)width;
        }
    }
}

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

    mydata.rays = new Ray[height*width];

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            mydata.rays[y*height + x] = cam.getRay(x, y);
        }
    }

    Tracer tracer(root, cam, ambient, lights);
    Image img(width, height, 3);

    mydata.tracer = &tracer;
    mydata.img = &img;

    mydata.width = width;
    mydata.height = height;

    i = 0;
    j = 0;
    k = 1;

    pthread_mutex_init(&mutex, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int a = 0; a < NUMTHREADS; a++) {
        pthread_create(&threads[a], &attr, computeImage, NULL);
    }

    pthread_attr_destroy(&attr);
    for(int a = 0; a < NUMTHREADS; a++) {
        int rc = pthread_join(threads[a], NULL);
        if(rc) {
            cout << "ERROR. Return code from pthread_join() is "
                << rc << endl;
            exit(-1);
        }
    }

    cout << "100\% complete" << endl;
    img.savePng(filename);

    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
}
