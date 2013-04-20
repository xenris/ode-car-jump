#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <ode/ode.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <cmath>
#include "drawable.hpp"
using namespace std;

#define WIDTH 1900
#define HEIGHT 900

#define NUM_CUBES 40

class State {
    public:
        float posx, posy, posz;
        float rotx, roty, rotz;

        bool wkey, akey, skey, dkey, ekey, gkey, shiftkey, spacekey;
        int simSpeed;

        bool carcam;

        SDL_Surface* screen;
        dWorldID world;
        dSpaceID worldSpace;

        Drawable* carbodydrawable;
        Drawable* carwheeldrawable;
        Drawable* cube;
        Drawable* map;
//        Drawable* monkey;

        dBodyID carbodybody;
        dBodyID flcarwheelbody;
        dBodyID frcarwheelbody;
        dBodyID blcarwheelbody;
        dBodyID brcarwheelbody;
        dBodyID cubebody[NUM_CUBES];

        const double* var;

//        dBodyID monkeyBody;
//        dGeomID carbodygeom;
//        dGeomID flcarwheelgeom;
//        dGeomID frcarwheelgeom;
//        dGeomID blcarwheelgeom;
//        dGeomID brcarwheelgeom;
//        dGeomID cubegeom;

        dJointGroupID physicsContactgroup;
};

State* init();
void uninit();
bool processInput(State* state);
void update(State* state);
void nearCallback(void *data, dGeomID o1, dGeomID o2);

#endif
