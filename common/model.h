#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

#include "../Graphics/shader.h"

#ifndef MODEL
#define MODEL

typedef struct {
    vec3 position;
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

void loadModel(Model* model, const char* path);
void drawModel(Model* model, SHADER shader);
void drawMesh(Mesh* mesh, SHADER shader);

#endif // MODEL
