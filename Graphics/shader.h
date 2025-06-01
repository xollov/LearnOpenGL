#ifndef SHADER_H
#define SHADER_H

#include<stdio.h>
#include <glad/glad.h>

typedef unsigned int SHADER; 
int readFromFile(FILE* from, char** to);
void s_set(SHADER* this, char* vertexPath, char* fragPath);
void s_use(SHADER this);
void s_setInt(SHADER this, char* name, int value);
void s_setFloat(SHADER this, char* name, float value);
void s_setVec4f(SHADER this, char* name, float x, float y, float z, float w);
void s_setVec3f(SHADER this, char* name, float x, float y, float z);
void s_setMatrix4fv(SHADER this, char* name, int count, int bool, float* value);
#endif // SHADER_H
