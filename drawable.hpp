#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <GL/gl.h>
#include <string.h>
#include <vector>
#include <string>
#include <ode/ode.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
using namespace std;

class Drawable {
    public:
//        GLuint glName;
        vector<GLfloat> vertices;
        vector<GLfloat> normals;
        vector<GLfloat> textureCoordinates;
//        float aspectRatio;
//        bool isFlatSprite;
//        bool hasTexture;

        Drawable(const char* fileAddress);
//        void loadTexture();
        void loadOBJ(const char* fileAddress);
        void draw(dBodyID body);
};

#endif
