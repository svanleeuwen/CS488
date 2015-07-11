#include "a4.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "tracer.hpp"
#include "primitive.hpp"
#include "packet.hpp"

#include <math.h>
#include <iostream>
#include <pthread.h>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

#define NUMTHREADS 8

namespace {
    int index;
    int k;

    vector<Packet*>* packets;

    pthread_t threads[NUMTHREADS];
    pthread_mutex_t mutex;
}

void* computeImage(void* arg) {
    (void)arg;

    int numPackets = packets->size();
    int l_index;

    while(true) {
        pthread_mutex_lock(&mutex);
        if(index >= numPackets) {
            pthread_mutex_unlock(&mutex);
            pthread_exit((void*) 0);
        }

        l_index = index++; 
        pthread_mutex_unlock(&mutex);

        packets->at(l_index)->trace();

        if(l_index == (int) ((k/10.0) * numPackets)) {
            cout << k*10 << "\% complete" << endl;
            k++;
        }
    }       
}

Primitive** unpackPrimitives(vector<Primitive*>* primitives) {
    Primitive** primArray = new Primitive* [primitives->size()];
    for(uint i = 0; i < primitives->size(); i++) {
        primArray[i] = primitives->at(i);
    }

    return primArray;
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

    vector<Primitive*>* primitives = new vector<Primitive*>();
    root->getPrimitives(primitives);
    delete root;
    
#ifndef BIH
    Tracer tracer(primitives, cam, ambient, lights);
#else
    Primitive** primArray = unpackPrimitives(primitives);
    BIHTree* bih = new BIHTree(primArray, primitives->size());
    
    delete primitives; 
    Tracer tracer(bih, cam, ambient, lights);
#endif

    Image img(width, height, 3);

    packets = Packet::genPackets(&img, &tracer, cam);
    index = 0;
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
