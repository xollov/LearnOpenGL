
#include "Graphics/shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"

#include <stdio.h>
#include <GLFW/glfw3.h> //-lglfw -lGL are required links

#include <cglm/cglm.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void procesInput(GLFWwindow* window);
void mousecallback(GLFWwindow* window, double xpos, double ypos);

vec3 cameraPos    = {0, 0, 5};  // position of camera 
vec3 cameraFront  = {0, 0, -1}; // where camera is looking
vec3 cameraUp     = {0, 1, 0};  // Up direction of the world

float cameraSpeed = 5;
float deltaTime   = 0; 
float lastFrame   = 0;

vec2 mouseLast    = {0, 0};    // last pos of mouse
float yaw = -90, pitch = 0;    // NOTE(xollow): learn what it is again

int main() {
  //glfw config
  if(!glfwInit()) {
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800,800,"LearnOpenGL", NULL, NULL); 
  if (window == NULL) {
      fprintf(stderr, "glfwCreateWindow() failed.\n");
      glfwTerminate();
      return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      fprintf(stderr,"gladLoadGLLoader() failed.\n");
  }
  glViewport(0,0,800,800); //glad
  
  
  glEnable(GL_DEPTH_TEST);
  //=======================================================================
  //SHADERS
  SHADER shader, lightShader;
  s_set(&shader, "VertexShader.vs", "FragShader.fs");
  s_set(&lightShader,"VertexShader.vs","LightFragShader.fs"); 
  // VBO & VAO & EBO
  // ---------------------------------------
  float vertices [] = {
    // pos               //tex         //color
    // front
    -.5f, -.5f, .5f,
    -.5f,  .5f, .5f,
     .5f, -.5f, .5f,
     .5f,  .5f, .5f,

    // top
    -.5f, .5f,  0.5f,
    -.5f, .5f, -0.5f,
     .5f, .5f,  0.5f,
     .5f, .5f, -0.5f,

    // right
     .5f, -.5f,  .5f,
     .5f,  .5f,  .5f,
     .5f, -.5f, -.5f,
     .5f,  .5f, -.5f,

    // left
     -.5f, -.5f,  .5f,
     -.5f,  .5f,  .5f,
     -.5f, -.5f, -.5f,
     -.5f,  .5f, -.5f,

    // bottom
     -.5f, -.5f,  .5f,
     -.5f, -.5f, -.5f,
      .5f, -.5f,  .5f,
      .5f, -.5f, -.5f,
    // back
     -.5f, -.5f, -.5f,
     -.5f,  .5f, -.5f,
      .5f, -.5f, -.5f,
      .5f,  .5f, -.5f,
  };
  unsigned int indices [] = {
    0,1,3,
    0,2,3,

    4,5,7,
    4,6,7,

    8,9,11,
    8,10,11,

    12,13,15,
    12,14,15,

    16,17,19,
    16,18,19,

    20, 21, 23,
    20, 22, 23,
  };
  

  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  //SHADERS END
  //=======================================================================

  // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
  // -------------------------------------------------------------------------------------------
  // don't forget to activate/use the shader before setting uniforms!
  // Texture END
  //=======================================================================
 

  vec3 cubePositions[] = {
  {0,  0,  0},
  {1,  2,  -1},
  };


  glfwSetKeyCallback(window,keycallback);
  
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mousecallback);

  float timer = 0;
  while (!glfwWindowShouldClose(window)) {

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    timer += deltaTime;
    procesInput(window);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // ----------------------------------------------------
    
    //float gamma = sin(glfwGetTime())/2 + 0.5f;
    // bind textures on corresponding texture units
    glUseProgram(shader);
    
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    mat4 view  = GLM_MAT4_IDENTITY_INIT;
    
    vec3 front;
    glm_vec3_add(cameraPos,cameraFront, front);
    
    /*
    if (timer > 2) {
      printf("CamerPos: {%f, %f, %f}  CameraFront{%f, %f, %f}\n",cameraPos[0], cameraPos[1], cameraPos[2],cameraFront[0], cameraFront[1], cameraFront[2]);
      printf("front: {%f, %f, %f}\n", front[0], front[1], front[2]);
      timer = 0;
    }
    */
    glm_lookat(cameraPos, front, cameraUp, view);
    glm_perspective(glm_rad(45), 1, 0.1f, 100.0f, projection);
    
    s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);
    s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);


    glBindVertexArray(VAO);
    mat4 model = GLM_MAT4_IDENTITY_INIT;  
    glm_translate(model,cubePositions[0]);
    s_setVec3f(shader, "objColor", 1, 0.5, 0.31);
    s_setVec3f(shader, "lightColor", 0.3,0.1,0.4);
    s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    glUseProgram(lightShader);
    glBindVertexArray(VAO);
    glm_mat4_ucopy(GLM_MAT4_IDENTITY, model);  
    glm_translate(model,cubePositions[1]);
    s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
    s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);
    s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
   /*
    */
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Dont touch yet
    glfwSwapBuffers(window);
    glfwPollEvents();    
  
    }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shader);
  glDeleteProgram(lightShader);
 
  glfwTerminate();
  return 0;
}

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  
  static int linemode = 1;
  static int cursor = 1;
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, (linemode = !linemode) ? GL_FILL : GL_LINE);
  }
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE); 
  }   

  if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    
  glfwSetInputMode(window, GLFW_CURSOR, (cursor = !cursor) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  }
}


// floating-like camera movement 
void procesInput(GLFWwindow* window) {
  
  cameraSpeed = 250 * deltaTime;
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 temp;
    glm_vec3_copy(cameraFront, temp);
    // glm_vec3_normalize(temp);
    // printf("CameraFront: {%f,%f,%f};  Normalized: {%f,%f,%f}\n", cameraFront[0],cameraFront[1],cameraFront[2], temp[0],temp[1],temp[2]);
    // cameraPos[2] -= cameraSpeed * deltaTime;
    glm_vec3_scale(temp, cameraSpeed * deltaTime, temp);
    glm_vec3_add(cameraPos, temp, cameraPos);

  }
  if (glfwGetKey(window,  GLFW_KEY_S) == GLFW_PRESS) {
    //cameraPos[2] += cameraSpeed * deltaTime;

    vec3 temp;
    glm_vec3_copy(cameraFront, temp);
    glm_vec3_scale(temp, -1 * cameraSpeed * deltaTime, temp);
    glm_vec3_add(cameraPos, temp, cameraPos);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 temp;
    glm_vec3_cross(cameraUp, cameraFront, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, cameraSpeed * deltaTime, temp);
    glm_vec3_add(cameraPos, temp, cameraPos);
  }
      
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 temp;
    glm_vec3_cross(cameraUp, cameraFront, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, -cameraSpeed * deltaTime, temp);
    glm_vec3_add(cameraPos, temp, cameraPos);
  }
}
void mousecallback(GLFWwindow* window, double xpos, double ypos) {

  static char firstMouse = 1;
  if (firstMouse) {
    
    mouseLast[0] = xpos;
    mouseLast[1] = ypos;
    firstMouse = 0;
  }
  
  float xoffset  = xpos - mouseLast[0];
  float yoffset = mouseLast[1] - ypos ;
  mouseLast[0] = xpos;
  mouseLast[1] = ypos;
  

  float sens = 0.01f;
  xoffset *= sens;
  yoffset *= sens;

  yaw += xoffset;
  pitch += yoffset;
  
  if (pitch > 89) pitch = 89;
  if (pitch < -89) pitch = -89;

  vec3 dir = {
    cos(glm_rad(yaw)) * cos(glm_rad(pitch)),
    sin(glm_rad(pitch)),
    sin(glm_rad(yaw)) * cos(glm_rad(pitch)),
  };
  glm_normalize(dir);
  glm_vec3_copy(dir, cameraFront);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
