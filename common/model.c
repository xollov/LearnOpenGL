#include "model.h"

#define MAX_TEXTURES 100
static Texture loaded_textures[MAX_TEXTURES];
static int loadedTexturesCount = 0;
static const char textureTypeNames[2][10] = {"diffuse", "specular"};

static char assetPath[256];

void loadMaterialTextures(struct aiMaterial* mat, enum aiTextureType type, Texture* texture, int* textureCount) {
   
    for (int i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
        struct aiString str; 
        aiGetMaterialTexture(mat, type, i, &str, NULL,NULL,NULL,NULL,NULL,NULL);
        int skip = 0;
        for (int j = 0; j < MAX_TEXTURES; j++) {
            if (strncmp(loaded_textures[j].path, str.data, str.length) == 0) {
                skip = 1;
                texture[*textureCount] = loaded_textures[j];
                (*textureCount)++;
                break;
            }
        }
        if (skip == 0) {
            Texture tex;        
            char path[256] = "Assets/Backpack/";
            
            tex.id = loadTexture(strcat(path, str.data)); 
            tex.type = type == aiTextureType_DIFFUSE ? diffuse : specular;  
            strcpy(tex.path, str.data);

            texture[(*textureCount)++] = tex;
            loaded_textures[loadedTexturesCount++] = tex;
            if (loadedTexturesCount >=  MAX_TEXTURES) {
               printf("EXCIDED MAXIMUM TEXTURE COUNT!!!!!!\n"); 
               loadedTexturesCount--; 
            }
        }
    }
}
Mesh processMesh(struct aiMesh* mesh, const struct aiScene* scene) {

    Mesh result;
    Vertex* vertices;
    unsigned int* indices;
    Texture* textures;
    struct aiMaterial* mat;

    unsigned int textureCount = 0;
    // allocate memory
    vertices = malloc(mesh->mNumVertices * sizeof(Vertex));
    int totalIndices = 0;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        totalIndices += mesh->mFaces[i].mNumIndices;
    }
    indices = malloc(totalIndices * sizeof(unsigned int));
    if (NULL == vertices || NULL == indices) {
        printf("failed to allocate memory for vertices or indices\n");
        result.vertices = NULL;
        result.indices = NULL;
        result.textures = NULL;
        result.verticesSize = 0;
        result.indicesSize = 0;
        result.texturesSize = 0;
        return result;
    }

    if (mesh->mMaterialIndex >= 0) {
        mat = scene->mMaterials[mesh->mMaterialIndex];
        textureCount += aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE); 
        textureCount += aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR); 
        textures = malloc(textureCount * sizeof(Texture)); 
    } else {
        mat = NULL;
    }
    if (textures == NULL) {
        printf("failed to allocatememory for textures\n");
    }

    // fill arrays
    for (int i = 0; i < mesh->mNumVertices; i++) {
        vertices[i].position[0] = mesh->mVertices[i].x;
        vertices[i].position[1] = mesh->mVertices[i].y;
        vertices[i].position[2] = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0]) {
            vertices[i].texCoords[0] = mesh->mTextureCoords[0][i].x;
            vertices[i].texCoords[1] = mesh->mTextureCoords[0][i].y;
        }
        else {
         vertices[i].texCoords[0] = 0;
         vertices[i].texCoords[1] = 0;
        }

    }
    int offset = 0;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices[offset++] = face.mIndices[j];
        }
    }
    if (NULL != mat) {
        
        int offset = 0;
        loadMaterialTextures(mat, aiTextureType_DIFFUSE, textures, &offset);
        loadMaterialTextures(mat, aiTextureType_SPECULAR, textures, &offset);
    }
    
    // result
    result.vertices = vertices;
    result.indices = indices;
    result.textures = textures;
    result.verticesSize = mesh->mNumVertices;
    result.indicesSize = totalIndices;
    result.texturesSize = textureCount;
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
    glBufferData(GL_ARRAY_BUFFER, mesh->verticesSize * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesSize * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3));

    glBindVertexArray(0);
}

// API model.h
void loadModel(Model* model, const char* path) {
    
    printf("Start model loading: %s\n", path);
    const struct aiScene* scene = aiImportFile(path, 
                                               aiProcess_Triangulate 
                                               | aiProcess_JoinIdenticalVertices
                                               ); 
    
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
void drawModel(Model* model, SHADER shader) {

    for (int i = 0; i < model->meshesSize; i++) {
        drawMesh(&model->meshes[i], shader);
    }
}

void drawMesh(Mesh* mesh, SHADER shader) {
    
    // textures
    unsigned int textureNr[2] = {1,1};

    for (int i = 0; i < mesh->texturesSize; i++) {

        glActiveTexture(GL_TEXTURE0 + i);
        Texture tex = mesh->textures[i];
        char buff[32];
        char number[2];
        sprintf(number, "%d", textureNr[tex.type]++);
        
        strcpy(buff, textureTypeNames[tex.type]);
        strcat(buff, number); 

        s_setInt(shader, buff, i); 
        glBindTexture(GL_TEXTURE_2D, tex.id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
// API model.h end
