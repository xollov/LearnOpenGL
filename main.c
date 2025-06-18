
#include "Graphics/shader.h"

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
    // pos            //norm   // tex
    // front
    -.5f, -.5f, .5f,   0, 0, 1,  0, 0, 
    -.5f,  .5f, .5f,   0, 0, 1,  0, 1,
     .5f, -.5f, .5f,   0, 0, 1,  1, 0,
     .5f,  .5f, .5f,   0, 0, 1,  1, 1,

    // top
    -.5f, .5f,  .5f,   0, 1, 0,  0, 0,
    -.5f, .5f, -.5f,   0, 1, 0,  0, 1,
     .5f, .5f,  .5f,   0, 1, 0,  1, 0,
     .5f, .5f, -.5f,   0, 1, 0,  1, 1,

    // right
     .5f, -.5f,  .5f,  1, 0, 0,  0, 0,
     .5f,  .5f,  .5f,  1, 0, 0,  0, 1,
     .5f, -.5f, -.5f,  1, 0, 0,  1, 0,
     .5f,  .5f, -.5f,  1, 0, 0,  1, 1,

    // left
    -.5f, -.5f,  .5f, -1, 0, 0,  1, 0,
    -.5f,  .5f,  .5f, -1, 0, 0,  1, 1,
    -.5f, -.5f, -.5f, -1, 0, 0,  0, 0,
    -.5f,  .5f, -.5f, -1, 0, 0,  0, 1,

    // bottom
    -.5f, -.5f,  .5f,  0, -1, 0,  0, 0,
    -.5f, -.5f, -.5f,  0, -1, 0,  0, 1,
     .5f, -.5f,  .5f,  0, -1, 0,  1, 0,
     .5f, -.5f, -.5f,  0, -1, 0,  1, 1,
    // back
    -.5f, -.5f, -.5f,  0, 0, -1,  0, 1, 
    -.5f,  .5f, -.5f,  0, 0, -1,  0, 0,
     .5f, -.5f, -.5f,  0, 0, -1,  1, 1,
     .5f,  .5f, -.5f,  0, 0, -1,  1, 0,
  };
  /*
  float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
  */
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
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  
  // light cube vao
  // NOTE(xollow): for every VAO you use you NEED to bind some VBO and EBO(opt.)
  // if you bind alredy existinh VBO and EBO you dont need to call glBufferData for either of them.
  unsigned int lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  //SHADERS END
  //=======================================================================
  unsigned int contTexture = loadTexture("container2.png"); 
  unsigned int specMapText = loadTexture("container2_specular.png");
  unsigned int emission    = loadTexture("container2_emission.png");
 
  glUseProgram(shader);
  s_setInt(shader, "material.diffuse", 0);
  s_setInt(shader, "material.specular", 1);
  s_setInt(shader, "material.emission", 2);
  // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
  // -------------------------------------------------------------------------------------------
  // don't forget to activate/use the shader before setting uniforms!
  // Texture END
  //=======================================================================
 

  vec3 cubePositions[] = {
  {0,  0,  0},
  {2,  5,  -15},
  {-1, -2.2, -12.4},
  {-3.8f, -2.0f, -12.3f},  
  { 2.4f, -0.4f, -3.5f},  
  {-1.7f,  3.0f, -7.5f},  
  { 1.3f, -2.0f, -2.5f},  
  { 1.5f,  2.0f, -2.5f}, 
  { 1.5f,  0.2f, -1.5f}, 
  {-1.3f,  1.0f, -1.5}
  };
  vec3 lightPositions[] = {
    {1,  0,  0.5}, // light cube
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

    glClearColor(0.3, 0.1, 0.6, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // ----------------------------------------------------
    
    //float gamma = sin(glfwGetTime())/2 + 0.5f;
    // bind textures on corresponding texture units
    
    // setup
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    mat4 view  = GLM_MAT4_IDENTITY_INIT;
    mat4 model = GLM_MAT4_IDENTITY_INIT;  

    vec3 front;
    glm_vec3_add(cameraPos, cameraFront, front);
    glm_lookat(cameraPos, front, cameraUp, view);
    glm_perspective(glm_rad(45), 1, 0.1f, 100.0f, projection);

    // set light source values
    vec3 lightColor = {1,1,1};
    vec3 diffuseColor = {0.5, 0.5, 0.5}, ambientColor = {.2, .2, .2};
    
    // light cube
    glUseProgram(lightShader);
    glm_translate(model,lightPositions[0]);
    vec3 lightPos;
    glm_vec3_copy(lightPositions[0], lightPos); 
    glm_translate(model, lightPos);
    vec3 scale = {0.3, 0.3, 0.3};
    glm_scale(model, scale); 
    s_setMatrix4fv(lightShader, "model", 1, GL_FALSE, model[0]);
    s_setMatrix4fv(lightShader, "view", 1, GL_FALSE, view[0]);
    s_setMatrix4fv(lightShader, "projection", 1, GL_FALSE, projection[0]);
    s_setVec3farr(lightShader,  "lightColor", lightColor);
    s_setVec3f(lightShader, "spotDir", 1, 0, 0);
    glBindVertexArray(lightVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    for (int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) {
      glUseProgram(shader);
      s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);
      s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
      glm_mat4_ucopy(GLM_MAT4_IDENTITY, model);
      glm_translate(model, cubePositions[i]);
      float angle = 20 * i;
      vec3 axis = {0.3, 0.4, -0.6};
      vec3 pivot = {0,0,0};
      glm_rotate_at(model, pivot, glfwGetTime() * glm_rad(angle), axis);
      //printf("%d.Cube rad: %f, time: %f, rotation: %f\n",i, glm_rad(angle), glfwGetTime(),  glfwGetTime() * glm_rad(angle));
      s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
      s_setVec3farr(shader, "light.position", cameraPos);
      s_setVec3farr(shader, "light.spotDirection", cameraFront);
      s_setFloat(shader, "light.cutOff", cos(glm_rad(12.5)));
      s_setFloat(shader, "light.outerCutOff", cos(glm_rad(17)));
      s_setVec3farr(shader, "light.ambient", ambientColor);
      s_setVec3farr(shader, "light.diffuse", diffuseColor);
      s_setVec3f(shader, "light.specular", 1, 1, 1);
      s_setFloat(shader, "light.constant", 1); 
      s_setFloat(shader, "light.linear", 0.14); 
      s_setFloat(shader, "light.quadratic", 0.07); 
      s_setVec3farr(shader, "viewPos", cameraPos);
      // set material values
      s_setFloat(shader, "material.shininess", 32);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, contTexture);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specMapText);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, emission);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    // Dont touch yet
    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteVertexArrays(1, &lightVAO);
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
    glm_vec3_scale(temp, cameraSpeed * deltaTime, temp);
    glm_vec3_add(cameraPos, temp, cameraPos);

  }
  if (glfwGetKey(window,  GLFW_KEY_S) == GLFW_PRESS) {
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
  // mode camera down 
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      vec3 temp;
      glm_vec3_scale(cameraUp, -cameraSpeed * deltaTime, temp);
      glm_vec3_add(cameraPos, temp, cameraPos);
  }
  // move camera up
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      vec3 temp;
      glm_vec3_scale(cameraUp, cameraSpeed * deltaTime, temp);
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
