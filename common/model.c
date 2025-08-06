#include "model.h"


Mesh processMesh(struct aiMesh* mesh, const struct aiScene* scene) {

    Mesh result;
    vec3* vertices;
    unsigned int* indices;

    // allocate memory
    vertices = malloc(mesh->mNumVertices * sizeof(vec3));
    int totalIndices = 0;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        totalIndices += mesh->mFaces[i].mNumIndices;
    }
    indices = malloc(totalIndices * sizeof(unsigned int));
    
    if (NULL == vertices || NULL == indices) {
        printf("failed to allocate memory for vertices or indices\n");
        result.vertices = NULL;
        result.indices = NULL;
        result.verticesSize = 0;
        result.indicesSize = 0;
        return result;
    }

    // fill arrays
    for (int i = 0; i < mesh->mNumVertices; i++) {
        vertices[i][0] = mesh->mVertices[i].x;
        vertices[i][1] = mesh->mVertices[i].y;
        vertices[i][2] = mesh->mVertices[i].z;
        printf("%f %f %f\n", vertices[i][0],vertices[i][1],vertices[i][2]);
    }
    int offset = 0;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices[offset++] = face.mIndices[j];
        }
    }
    
    // result
    result.vertices = vertices;
    result.indices = indices;
    result.verticesSize = mesh->mNumVertices;
    result.indicesSize = totalIndices;
    return result;
}

void processNode(struct aiNode* node, const struct aiScene* scene, Model* model, int* meshesProcessed) {

    for (int i = 0; i < node->mNumMeshes; i++) {

        Mesh mesh = processMesh(scene->mMeshes[node->mMeshes[i]], scene);
        model->meshes[*meshesProcessed] = mesh; 
        (*meshesProcessed)++;
    }
    for (int i = 0; i < node->mNumChildren; i++){

        processNode(node->mChildren[i], scene, model, meshesProcessed);
    }
}
void setupMesh(Mesh* mesh) {
       
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    
    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->verticesSize * sizeof(vec3), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesSize * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
}

// API model.h
void loadModel(Model* model, const char* path) {
    
    printf("Start model loading: %s\n", path);
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate); 
    
    model->meshes = malloc(scene->mNumMeshes * sizeof(Mesh));
    printf("Number of meshes: %d\n", scene->mNumMeshes);
    if (NULL == model->meshes) {

        printf("failed to allocate memory for meshes.\n");
    }
    struct aiNode* root = scene->mRootNode;
    int meshesProcessed = 0;
    processNode(root, scene, model, &meshesProcessed);
    
    // check if processed correctly
    model->meshesSize = scene->mNumMeshes; 
    for (int i = 0; i <  model->meshesSize; i++) {
        //printf("%d.Meshe vertices: %d, indices: %d\n", i, model->meshes[i].verticesSize, model->meshes[i].indicesSize);
        //printf("%d mesh check, vertices: %d, indices: %d\n", i, scene->mMeshes[i]->mNumVertices, scene->mMeshes[i]->mNumFaces * 3);
        setupMesh(&model->meshes[i]); 
    }

    aiReleaseImport(scene);
}
void drawModel(Model* model) {

    for (int i = 0; i < model->meshesSize; i++) {
        drawMesh(&model->meshes[i]);
    }
}

void drawMesh(Mesh* mesh) {
    
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
// API model.h end
