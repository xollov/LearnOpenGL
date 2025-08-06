#include "Graphics/shader.h"

#include <stdio.h>
#include <GLFW/glfw3.h> //-lglfw -lGL are required links

#include <cglm/cglm.h>
#include "common/model.h"

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

    GLFWwindow* window = glfwCreateWindow(1920, 1080,"LearnOpenGL", NULL, NULL); 
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
    glViewport(0,0, 1920, 1080); //glad
    glEnable(GL_DEPTH_TEST);
    //
    //=======================================================================
    //SHADERS
    SHADER shader, lightShader;
    s_set(&shader, "VertexShader.glsl", "FragShader.glsl");
    s_set(&lightShader,"VertexShader.glsl","LightFragShader.glsl"); 
    
    
    //=======================================================================
    // MODEL LOADING
     
    Model cubeModel;
    loadModel(&cubeModel, "Assets/Backpack/backpack.obj"); 
    //=======================================================================
    // VBO & VAO & EBO
   
    //=======================================================================
    //
    glfwSetKeyCallback(window, keycallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mousecallback);

    float timer = 0;
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.1, 0.7, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        procesInput(window);
        // ----------------------------------------------------

        glUseProgram(shader);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;

        vec3 front;
        glm_vec3_add(cameraPos, cameraFront, front);
        glm_lookat(cameraPos, front, cameraUp, view);
        glm_perspective(glm_rad(45), 1, 0.1f, 100.0f, projection);
       
        vec3 pos = {0,0,-3};
        glm_mat4_scale(model, 0.3); 
        glm_translate(model, pos);
        float angle = 20;
        vec3 axis = {0.3, 0.4, -0.6};
        vec3 pivot = {0,0,0};
        glm_rotate_at(model, pivot, glfwGetTime() * glm_rad(angle), axis);
        


        s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
        s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);
        s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
        drawModel(&cubeModel);


        // Dont touch yet
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

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
