#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#include "shader.h"

#ifndef MODEL
#define MODEL

typedef struct {
    vec3 position;
    vec3 normals;
    vec2 texCoords;
} Vertex;

enum TextureType {
    diffuse,
    specular
};

typedef struct {
    enum TextureType type;
    unsigned int id;
    char path[256];
} Texture;

typedef struct {

    Vertex* vertices;
    unsigned int* indices;
    Texture* textures;
    int verticesSize, indicesSize, texturesSize;
    unsigned int VAO,VBO,EBO;
} Mesh;

typedef struct {

    Mesh* meshes;
    int meshesSize;
} Model;

void loadModel(Model* model, const char* modelPath, const char* absolutePath);
void drawModel(Model* model, SHADER shader);
void drawMesh(Mesh* mesh, SHADER shader);
void deleteModel(Model* model);
#endif // MODEL
