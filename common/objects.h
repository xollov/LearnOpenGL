
#ifndef OBJECTS
#define OBJECTS

enum ObjectTriangles {
    PLANE,
    CUBE,
};
extern int trianglesCount[];
extern unsigned int planeVAO, planeVBO,
                    cubeVAO,  cubeVBO,
                    quadVAO, quadVBO,
                    skyboxVAO, skyboxVBO;
void objectsInit();
void objectsDelete();
#endif // OBJECTS
