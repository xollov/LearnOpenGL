#include "shader.h"
#include<fcntl.h>
#include<stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int readFromFile(FILE* from, char** to){
  if (from == NULL) {
    printf("Source file doesnt exist or couldnt be open\n");  
    return 1;
  }
  fseek(from, 0, SEEK_END);
  long size = ftell(from);
  fseek(from, 0, SEEK_SET);
  *to = malloc(size+1);
  fread(*to, 1, size, from);
  (*to)[size] = '\0';
  return 0;
}
void s_set(SHADER* this, char* vertexPath, char* fragPath) {  
  FILE *vertexFile, *fragFile;
  char* vertexSource;
  char* fragSource;

  vertexFile = fopen(vertexPath,  "rb");
  fragFile   = fopen(fragPath,    "rb");

  if (readFromFile(vertexFile, &vertexSource) != 0) {
    printf("Coudnt read vertex shader source file\n");
    return;
  }
  if (readFromFile(fragFile, &fragSource) != 0) {
    printf("Coudnt read fragment shader source file\n");
    return;
  }
  
  printf("-\"%s\" loaded successfully.\n"  ,vertexPath);
  printf("-\"%s\" loaded successfully.\n",fragPath);
  
  
  fclose(vertexFile);
  fclose(fragFile);

  unsigned int vertexShader,fragShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (void*) &vertexSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infolog[512];
  //vertex shader
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
      printf("Vertex shader vertex compilation failed.\n%s\n", infolog);
  }
  //fragment shader
  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, (void*) &fragSource, NULL);
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
  if (!success) {
      glGetShaderInfoLog(fragShader, 512, NULL, infolog);
      printf("Fragment shader fragment compilation failed.\n%s\n", infolog);
  }
  //linking to shader program
  *this = glCreateProgram();
  glAttachShader(*this, vertexShader);
  glAttachShader(*this, fragShader);
  glLinkProgram (*this);
  
  glGetProgramiv(*this, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(*this, 512, NULL, infolog);
    printf("Shader link failed.\n%s\n",infolog);
  }
  
  // free resources
  free(vertexSource);
  free(fragSource);
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);
}
unsigned int loadTexture(const char* path) {
  
  unsigned int texture;
  glGenTextures(1, &texture);

  int width, height, nrchannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrchannels, 0);
  if (data)
  {
    GLenum format;
    switch (nrchannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4: 
        format = GL_RGBA;
    }
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    printf("Texture loaded: %s\n", path);
  }
  else
  {
    printf("failed to load texture. %s\n", path);
  }
  stbi_image_free(data);
  return texture; 
}
void s_setInt(SHADER this, char* name, int value) {
 glUniform1i(glGetUniformLocation(this,name), value);
}
void s_setFloat(SHADER this, char* name, float value) {
  glUniform1f(glGetUniformLocation(this, name), value);
}
void s_setVec3f(SHADER this, char* name, float x, float y, float z) {
  glUniform3f(glGetUniformLocation(this, name), x, y, z);
}
void s_setVec3farr(SHADER this, char* name, float *arr) {
  glUniform3f(glGetUniformLocation(this, name), arr[0], arr[1], arr[2]);
}
void s_setVec4f(SHADER this, char* name, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(this, name), x, y, z, w);
}
void s_setMatrix4fv(SHADER this, char* name, int count, int flag, float* value) {
  glUniformMatrix4fv(glGetUniformLocation(this, name), 1, flag, value);
}
