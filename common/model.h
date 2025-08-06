#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#ifndef MODEL
#define MODEL

typedef struct {

    vec3* vertices;
    unsigned int* indices;
    int verticesSize, indicesSize;
    unsigned int VAO,VBO,EBO;
} Mesh;

typedef struct {

    Mesh* meshes;
    int meshesSize;
} Model;

void loadModel(Model* model, const char* path);
void drawModel(Model* model);
void drawMesh(Mesh* mesh);

#endif // MODEL
