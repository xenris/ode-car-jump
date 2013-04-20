#include "drawable.hpp"

Drawable::Drawable(const char* fileAddress) {
    loadOBJ(fileAddress);

//    hasTexture = true;
}

//void ACDrawable::loadTexture() {
//    string pngAddress = fileAddress + ".png";

//    glGenTextures(1, &glName);
//    glBindTexture(GL_TEXTURE_2D, glName);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

//    int error;
//    unsigned char* image;
//    unsigned int width, height;
//    unsigned char* png;
//    int pngsize;

//    png = acAssets->getFile(pngAddress.data(), &pngsize);
//    if(png == NULL) {
//        hasTexture = false;
//        return;
//    }

//    error = lodepng_decode32(&image, &width, &height, png, pngsize);
//    if(error) acLogError("error %u: %s\n", error, lodepng_error_text(error));

//    aspectRatio = (float)width/height;

//    free(png);

//    //scale image so that width and height are power of 2.
//    //This needs to be done for opengl es 1.1.
//    int width2 = 1;
//    int height2 = 1;
//    while(width2 < width)
//        width2 *= 2;
//    while(height2 < height)
//        height2 *= 2;
//    if(width2 != width || height2 != height) {
//        uint32_t* image2 = (uint32_t*)malloc(width2 * height2 * 4);
//        uint32_t* image3 = (uint32_t*)image;
//        float x_ratio = (float)width/(float)width2;
//        float y_ratio = (float)height/(float)height2;
//        float px, py;
//        int i, j;
//        for(i = 0; i < height2; i++) {
//            for(j = 0; j < width2; j++) {
//                px = (uint32_t)((float)j*x_ratio);
//                py = (uint32_t)((float)i*y_ratio);
//                image2[(i*width2)+j] = image3[(int)((py*width)+px)];
//            }
//        }

//        free(image);
//        image = (unsigned char*)image2;
//    }

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

//    free(image);
//}

void Drawable::loadOBJ(const char* fileAddress) {
    ifstream iofile(fileAddress, ios::in|ios::binary|ios::ate);
    if(!iofile.is_open()) {
        cout << "file not open" << endl;
        return;
    }
//    iofile.open(fileAddress);
//    vector<char> objFile;
    int fileSize = iofile.tellg();
    char* objFile = new char[fileSize];
    iofile.seekg(0, ios::beg);

    iofile.read(objFile, fileSize);

    iofile.close();
//    objFile.push_back(fgetc(iofile));


//    char* objFile = (char*)acAssets->getFile(objAddress.data(), &fileSize);

    vector<GLfloat> v;
    vector<GLfloat> vt;
    vector<GLfloat> vn;

//    for(int i = 0; i < 50; i++)
//        cout << (int)objFile[i];
//    return;

    for(int i = 0; i < fileSize; i++) {
        if((objFile[i] == 'v') && (objFile[i+1] == ' ')) {
            i+=2;
            for(int k = 0; k < 3; k++, i++) {
                v.push_back(atof(&objFile[i]));
                while((objFile[i] != ' ') && (objFile[i] != '\n')) i++;
            }
            i--;
        } else if((objFile[i] == 'v') && (objFile[i+1] == 't')) {
            i+=3;
            for(int k = 0; k < 2; k++, i++) {
                vt.push_back(atof(&objFile[i]));
                while((objFile[i] != ' ') && (objFile[i] != '\n')) i++;
            }
            i--;
        } else if((objFile[i] == 'v') && (objFile[i+1] == 'n')) {
            i+=3;
            for(int k = 0; k < 3; k++, i++) {
                vn.push_back(atof(&objFile[i]));
                while((objFile[i] != ' ') && (objFile[i] != '\n')) i++;
            }
            i--;
        } else if((objFile[i] == 'f') && (objFile[i+1] == ' ')) {
            i+=2;
            for(int k = 0; k < 3; k++, i++) {
                int vi = atoi(&objFile[i]) - 1;
                vertices.push_back(v[vi*3+0]);
                vertices.push_back(v[vi*3+1]);
                vertices.push_back(v[vi*3+2]);

                while((objFile[i] != '/') && (objFile[i] != ' ') && (objFile[i] != '\n')) i++;

                if(objFile[i] == '/') {
                    i++;
                    if(objFile[i] == '/') {
                        i++;
                        int vni = atoi(&objFile[i]) - 1;
                        normals.push_back(vn[vni*3+0]);
                        normals.push_back(vn[vni*3+1]);
                        normals.push_back(vn[vni*3+2]);
                        while((objFile[i] != '/') && (objFile[i] != ' ') && (objFile[i] != '\n')) i++;
                    } else {
                        int vti = atoi(&objFile[i]) - 1;
                        textureCoordinates.push_back(vt[vti*2+0]);
                        textureCoordinates.push_back(1.0 - vt[vti*2+1]);
                        while((objFile[i] != '/') && (objFile[i] != ' ') && (objFile[i] != '\n')) i++;
                        if(objFile[i] == '/') {
                            i++;
                            int vni = atoi(&objFile[i]) - 1;
                            normals.push_back(vn[vni*3+0]);
                            normals.push_back(vn[vni*3+1]);
                            normals.push_back(vn[vni*3+2]);
                            while((objFile[i] != '/') && (objFile[i] != ' ') && (objFile[i] != '\n')) i++;
                        }
                    }
                }
            }
            i--;
        } else {
            while(objFile[i] != '\n') i++;
        }
    }
}

void Drawable::draw(dBodyID body) {
//    if(hasTexture && !glIsTexture(glName))
//        loadTexture();

    glPushMatrix();

    if(body != 0) {
        const dReal* pos = dBodyGetPosition(body);
        const dReal* rot = dBodyGetRotation(body);
        const float matrix[] = { rot[0], rot[4], rot[8], 0
                               , rot[1], rot[5], rot[9], 0
                               , rot[2], rot[6], rot[10], 0
                               , pos[0], pos[1], pos[2], 1 };
        glMultMatrixf(matrix);
    }

//    glTranslatef(position->x, position->y, position->z);
//    glScalef(size->x, size->y, size->z);
//    if(isFlatSprite)
//        glScalef(aspectRatio, 1, 1);
//    glRotatef(rotation->x, 1, 0, 0);
//    glRotatef(rotation->y, 0, 1, 0);
//    glRotatef(rotation->z, 0, 0, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
//    if(hasTexture) {
//        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//        glBindTexture(GL_TEXTURE_2D, glName);
//        glTexCoordPointer(2, GL_FLOAT, 0, &textureCoordinates[0]);
//    }
    if(normals.size() > 0) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, &normals[0]);
    }

    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);

//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}
