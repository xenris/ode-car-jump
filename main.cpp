#include "main.hpp"

int main() {
    State* state = init();

    while(true) {
        if(!processInput(state))
            break;

        update(state);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if(state->carcam) {
            const dReal* pos = dBodyGetPosition(state->carbodybody);
            const dReal* rot = dBodyGetRotation(state->carbodybody);

            glRotatef(state->rotx, 1, 0, 0);
            glRotatef(state->roty, 0, 1, 0);
            glRotatef(state->rotz, 0, 0, 1);
            const float matrix[] = { rot[0], rot[1], rot[2], 0
                                   , rot[4], rot[5], rot[6], 0
                                   , rot[8], rot[9], rot[10], 0
                                   , 0, 0, 0, 1 };
            glMultMatrixf(matrix);
            glTranslatef(-pos[0], -pos[1], -pos[2]);
        } else {
            glRotatef(state->rotx, 1, 0, 0);
            glRotatef(state->roty, 0, 1, 0);
            glRotatef(state->rotz, 0, 0, 1);
            glTranslatef(-state->posx, -state->posy, -state->posz);
        }

        state->carbodydrawable->draw(state->carbodybody);
        state->carwheeldrawable->draw(state->flcarwheelbody);
        state->carwheeldrawable->draw(state->frcarwheelbody);
        state->carwheeldrawable->draw(state->blcarwheelbody);
        state->carwheeldrawable->draw(state->brcarwheelbody);
//        state->monkey->draw(state->monkeyBody);
        for(int i = 0; i < NUM_CUBES; i++)
            state->cube->draw(state->cubebody[i]);
        state->map->draw(0);
        SDL_GL_SwapBuffers();
    }

    uninit();
    return 0;
}

State* init() {
    State* state = new State();
    dInitODE();

    SDL_Init(SDL_INIT_EVERYTHING);

    state->screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL);
    SDL_WM_SetCaption("Physics", NULL);
    SDL_Flip(state->screen);

    SDL_ShowCursor(SDL_DISABLE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, (float)WIDTH/HEIGHT, 0.5, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_ambient[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    GLfloat lightpos[] = {0, 4, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 1);

    state->posx = 0;//21;
    state->posy = 4;//8;
    state->posz = 5;//21;
    state->rotx = 0;
    state->roty = 0;//-40;
    state->rotz = 0;

    state->wkey = false;
    state->akey = false;
    state->skey = false;
    state->dkey = false;
    state->gkey = false;

    state->simSpeed = 60;

    state->carcam = false;

    state->carbodydrawable = new Drawable("objs/carbody.obj");
    state->carwheeldrawable = new Drawable("objs/carwheel.obj");
    state->map = new Drawable("objs/jump2.obj");
    state->cube = new Drawable("objs/cube.obj");
//    state->monkey = new Drawable("objs/monkey.obj");

    state->world = dWorldCreate();
    dWorldSetGravity(state->world, 0, -9.8, 0);

    state->worldSpace = dHashSpaceCreate(0);

    const double carHeight = 0.65;
    const double carZ = 90;
    const double carX = 0;
    const float speed = -1000;
    const float force = 200;

    state->carbodybody = dBodyCreate(state->world);
    dBodySetPosition(state->carbodybody, carX, carHeight, carZ);
    dMass bodymass;
    dMassSetBoxTotal(&bodymass, 100, 2, 4, 1);
    dBodySetMass(state->carbodybody, &bodymass);
    dGeomID carbodygeom = dCreateBox(state->worldSpace, 2, 1, 4);
    dGeomSetBody(carbodygeom, state->carbodybody);

    state->flcarwheelbody = dBodyCreate(state->world);
    dBodySetPosition(state->flcarwheelbody, carX-1.5, carHeight - 0.5, carZ+2);
    const dMatrix3 m = { 0, 0, 1, 0
                       , 0, 1, 0, 0
                       , 0, 0, 1, 0 };
    dBodySetRotation(state->flcarwheelbody, m);
    dMass wheelmass;
    dMassSetCylinder(&wheelmass, 0.1, 2, 0.5, 0.2);
    dBodySetMass(state->flcarwheelbody, &wheelmass);
    dJointID joint = dJointCreateHinge(state->world, 0);
    dJointAttach(joint, state->carbodybody, state->flcarwheelbody);
    dJointSetHingeAnchor(joint, carX-1.5, carHeight - 0.5, carZ+2);
    dJointSetHingeAxis(joint, 1, 0, 0);
    dGeomID flcarwheelgeom = dCreateCylinder(state->worldSpace, 0.5, 0.2);
    dGeomSetBody(flcarwheelgeom, state->flcarwheelbody);

    dJointID motor = dJointCreateAMotor(state->world, 0);
    dJointAttach(motor, state->flcarwheelbody, state->carbodybody);
    dJointSetAMotorNumAxes(motor, 1);
    dJointSetAMotorAxis(motor, 0, 1, 1, 0, 0);
    dJointSetAMotorParam(motor, dParamVel, speed);
    dJointSetAMotorParam(motor, dParamFMax, force);

    state->frcarwheelbody = dBodyCreate(state->world);
    dBodySetPosition(state->frcarwheelbody, carX+1.5, carHeight - 0.5, carZ+2);
    dBodySetRotation(state->frcarwheelbody, m);
    dBodySetMass(state->frcarwheelbody, &wheelmass);
    joint = dJointCreateHinge(state->world, 0);
    dJointAttach(joint, state->carbodybody, state->frcarwheelbody);
    dJointSetHingeAnchor(joint, carX+1.5, carHeight - 0.5, carZ+2);
    dJointSetHingeAxis(joint, 1, 0, 0);
    dGeomID frcarwheelgeom = dCreateCylinder(state->worldSpace, 0.5, 0.2);
    dGeomSetBody(frcarwheelgeom, state->frcarwheelbody);

    motor = dJointCreateAMotor(state->world, 0);
    dJointAttach(motor, state->frcarwheelbody, state->carbodybody);
    dJointSetAMotorNumAxes(motor, 1);
    dJointSetAMotorAxis(motor, 0, 1, 1, 0, 0);
    dJointSetAMotorParam(motor, dParamVel, speed);
    dJointSetAMotorParam(motor, dParamFMax, force);

    state->blcarwheelbody = dBodyCreate(state->world);
    dBodySetPosition(state->blcarwheelbody, carX-1.5, carHeight - 0.5, carZ-2);
    dBodySetRotation(state->blcarwheelbody, m);
    dBodySetMass(state->blcarwheelbody, &wheelmass);
    joint = dJointCreateHinge(state->world, 0);
    dJointAttach(joint, state->carbodybody, state->blcarwheelbody);
    dJointSetHingeAnchor(joint, carX-1.5, carHeight - 0.5, carZ-2);
    dJointSetHingeAxis(joint, 1, 0, 0);
    dGeomID blcarwheelgeom = dCreateCylinder(state->worldSpace, 0.5, 0.2);
    dGeomSetBody(blcarwheelgeom, state->blcarwheelbody);

    motor = dJointCreateAMotor(state->world, 0);
    dJointAttach(motor, state->blcarwheelbody, state->carbodybody);
    dJointSetAMotorNumAxes(motor, 1);
    dJointSetAMotorAxis(motor, 0, 1, 1, 0, 0);
    dJointSetAMotorParam(motor, dParamVel, speed);
    dJointSetAMotorParam(motor, dParamFMax, force);

    state->brcarwheelbody = dBodyCreate(state->world);
    dBodySetPosition(state->brcarwheelbody, carX+1.5, carHeight - 0.5, carZ-2);
    dBodySetRotation(state->brcarwheelbody, m);
    dBodySetMass(state->brcarwheelbody, &wheelmass);
    joint = dJointCreateHinge(state->world, 0);
    dJointAttach(joint, state->carbodybody, state->brcarwheelbody);
    dJointSetHingeAnchor(joint, carX+1.5, carHeight - 0.5, carZ-2);
    dJointSetHingeAxis(joint, 1, 0, 0);
    dGeomID brcarwheelgeom = dCreateCylinder(state->worldSpace, 0.5, 0.2);
    dGeomSetBody(brcarwheelgeom, state->brcarwheelbody);

    motor = dJointCreateAMotor(state->world, 0);
    dJointAttach(motor, state->brcarwheelbody, state->carbodybody);
    dJointSetAMotorNumAxes(motor, 1);
    dJointSetAMotorAxis(motor, 0, 1, 1, 0, 0);
    dJointSetAMotorParam(motor, dParamVel, speed);
    dJointSetAMotorParam(motor, dParamFMax, force);

    state->var = new double[3];

    state->var = dBodyGetPosition(state->carbodybody);
//    cout << state->var[0] << " " << state->var[1] << " " << state->var[2] << endl;
    //TODO check if translation matrix from dBody can be used.

    dSpaceID cubespace = dHashSpaceCreate(state->worldSpace);

    for(int i = 0; i < NUM_CUBES/10; i++) {
        for(int k = 0; k < 10; k++) {
            state->cubebody[i*10+k] = dBodyCreate(state->world);
            dBodySetAutoDisableFlag(state->cubebody[i*10+k], 1);
            dBodySetPosition(state->cubebody[i*10+k], (i*2.01)-4, 0.9 + 2.01*k, -70);
            dGeomID cubegeom = dCreateBox(cubespace, 2, 2, 2);
            dGeomSetBody(cubegeom, state->cubebody[i*10+k]);
        }
    }

    {
        int indexSize = state->map->vertices.size()/3;
        unsigned int* index = new unsigned int[indexSize];
        for(int i = 0; i < indexSize; i++)
            index[i] = i;

        dTriMeshDataID triMeshData = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(triMeshData, state->map->vertices.data(), 12, state->map->vertices.size()/3, index, indexSize, 12);
        dGeomID mapGeom = dCreateTriMesh(state->worldSpace, triMeshData, NULL, NULL, NULL);
        dGeomSetPosition(mapGeom, 0, 0, 0);
    }
//    state->monkeyBody = dBodyCreate(state->world);
//    {
//        int indexSize = state->monkey->vertices.size()/3;
//        unsigned int* index = new unsigned int[indexSize];
//        for(int i = 0; i < indexSize; i++)
//            index[i] = i;

//        dTriMeshDataID triMeshData = dGeomTriMeshDataCreate();
//        dGeomTriMeshDataBuildSingle(triMeshData, state->monkey->vertices.data(), 12, state->monkey->vertices.size()/3, index, indexSize, 12);
//        dGeomID monkeyGeom = dCreateTriMesh(state->worldSpace, triMeshData, NULL, NULL, NULL);
//        dGeomSetPosition(monkeyGeom, 0, 2, 0);
//        dGeomSetBody(monkeyGeom, state->monkeyBody);
//    }

    state->physicsContactgroup = dJointGroupCreate(0);

    return state;
}

void uninit() {
    dCloseODE();
    SDL_Quit();
}

bool processInput(State* state) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            return false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_q)) {
            return false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_ESCAPE)) {
            return false;
        } else if(event.type == SDL_MOUSEMOTION) {
            if((event.motion.x != WIDTH/2) || (event.motion.y != HEIGHT/2)) {
                state->roty += (event.motion.x - WIDTH/2)/30.0;
                if(state->roty < 0)
                    state->roty = 360;
                state->rotx += (event.motion.y - HEIGHT/2)/30.0;
                if(state->rotx < 0)
                    state->rotx = 360;
                SDL_WarpMouse(WIDTH/2, HEIGHT/2);
            }
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_w)) {
            state->wkey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_a)) {
            state->akey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_s)) {
            state->skey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_d)) {
            state->dkey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_e)) {
            state->ekey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_g)) {
            state->gkey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_LSHIFT)) {
            state->shiftkey = true;
        } else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_SPACE)) {
            state->spacekey = true;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_w)) {
            state->wkey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_a)) {
            state->akey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_s)) {
            state->skey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_d)) {
            state->dkey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_e)) {
            state->ekey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_g)) {
            state->gkey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_c)) {
            state->carcam = !state->carcam;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_LSHIFT)) {
            state->shiftkey = false;
        } else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_SPACE)) {
            state->spacekey = false;
        } else if(event.type == SDL_MOUSEBUTTONUP) {
            if(event.button.button == 4) {
                state->simSpeed+=2;
                if(state->simSpeed > 120)
                    state->simSpeed = 120;
                cout << state->simSpeed << endl;
            } else if(event.button.button == 5) {
                state->simSpeed-=2;
                if(state->simSpeed < 1)
                    state->simSpeed = 1;
                cout << state->simSpeed << endl;
            }
        }
    }

    return true;
}

void update(State* state) {
    float speed;
    if(state->ekey)
        speed = 0.5;
    else
        speed = 0.3;

    if(state->wkey) {
        state->posz -= cos(state->roty*3.1415/180) * speed;
        state->posx += sin(state->roty*3.1415/180) * speed;
    }
    if(state->akey) {
        state->posz += cos((state->roty + 90)*3.1415/180) * speed;
        state->posx -= sin((state->roty + 90)*3.1415/180) * speed;
    }
    if(state->skey) {
        state->posz += cos(state->roty*3.1415/180) * speed;
        state->posx -= sin(state->roty*3.1415/180) * speed;
    }
    if(state->dkey) {
        state->posz -= cos((state->roty + 90)*3.1415/180) * speed;
        state->posx += sin((state->roty + 90)*3.1415/180) * speed;
    }
    if(state->shiftkey) {
        state->posy -= speed;
    }
    if(state->spacekey) {
        state->posy += speed;
    }

    if(state->gkey) {
        for(int i = 0; i < state->simSpeed; i++) {
            dSpaceCollide(state->worldSpace, state, &nearCallback);

            dWorldQuickStep(state->world, 0.015/60);

            dJointGroupEmpty(state->physicsContactgroup);

//            cout << state->var[0] << " " << state->var[1] << " " << state->var[2] << endl;
        }

//        state->gkey = false;
    }
}

void nearCallback(void *data, dGeomID o1, dGeomID o2) {
    State* state = (State*)data;

    if(dGeomIsSpace(o1) || dGeomIsSpace(o2)) {
        dSpaceCollide2(o1, o2, data, &nearCallback);

        if(dGeomIsSpace(o1))
            dSpaceCollide((dSpaceID)o1, data, &nearCallback);
        if(dGeomIsSpace(o2))
            dSpaceCollide((dSpaceID)o2, data, &nearCallback);
    } else {
        dBodyID b1 = dGeomGetBody(o1);
        dBodyID b2 = dGeomGetBody(o2);

        const int MAX_CONTACTS = 18;
        dContact contact[MAX_CONTACTS];

        for(int i = 0; i < MAX_CONTACTS; i++) {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = 2000;
            contact[i].surface.bounce = 0.1;
            contact[i].surface.bounce_vel = 0.15;
        }

        if(int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact))) {
            for(int i = 0; i < numc; i++) {
                dJointID c = dJointCreateContact(state->world, state->physicsContactgroup, &contact[i]);
                dJointAttach(c, b1, b2);
            }
        }
    }
}
