
#include "common/shader.h"
#include <stdio.h>
#include <GLFW/glfw3.h> //-lglfw -lGL are required links

#include <cglm/cglm.h>
#include "common/model.h"
#include "common/objects.h"

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

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;


// Note(): setting glfw_resizable to false or floating to true makes  widnow shrink into a line
int main() {
    //glfw config
    if(!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_FLOATING, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600,"LearnOpenGL", NULL, NULL); 
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
    glViewport(0,0, SCR_WIDTH, SCR_HEIGHT); //glad
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //=======================================================================
    //SHADERS
    SHADER shader, shaderSkybox;
    s_set(&shader, "Shaders/VertexReflection.glsl", "Shaders/FragReflection.glsl");
    s_set(&shaderSkybox, "Shaders/VertexSkybox.glsl", "Shaders/FragSkybox.glsl"); 
     
    //=======================================================================
    // MODEL LOADING
     
    Model backpackModel;
    loadModel(&backpackModel, "Assets/Backpack/backpack.obj", "Assets/Backpack/"); 

    objectsInit();
    unsigned int skybox = loadCubemap("Assets/skybox/skybox_");

    //=======================================================================
    //
    glfwSetKeyCallback(window, keycallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mousecallback);

   // float timer = 0;
    while (!glfwWindowShouldClose(window)) {

        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();
        procesInput(window);
        
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.5f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ----------------------------------------------------
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;

        vec3 front;
        glm_vec3_add(cameraPos, cameraFront, front);
        glm_lookat(cameraPos, front, cameraUp, view);
        glm_perspective(glm_rad(45), 1, 0.1f, 100.0f, projection);


        mat4 skyboxView;
        glm_mat4_copy(view, skyboxView);
        for (int i = 0; i < 3; i++) {
            skyboxView[i][3] = 0;
        }
         for (int j = 0; j < 3; j++) {
            skyboxView[3][j] = 0;
        }
        glDepthMask(GL_FALSE);
        glUseProgram(shaderSkybox);
        s_setMatrix4fv(shaderSkybox, "projection", 1, GL_FALSE, projection[0]);
        s_setMatrix4fv(shaderSkybox, "view", 1, GL_FALSE, skyboxView[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        glBindVertexArray(skyboxVAO); 
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glDepthMask(GL_TRUE);
        glUseProgram(shader);
        s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
        s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);

        // cube
        glm_mat4_identity(model);
        //vec3 axis = {0,1,0};
        //glm_rotate(model, glfwGetTime() * 0.5f, axis);
        s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
        s_setVec3farr(shader, "cameraPos", cameraPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0,36);
        
        // backpack
        glUseProgram(shader);
        s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
        s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);

        glm_mat4_identity(model);
        vec3 pos = {10,0,0};
        glm_translate(model,pos); 
        vec3 axis = {0,1,0};
        glm_rotate(model, glfwGetTime() * 0.5f, axis);
        s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        s_setVec3farr(shader, "cameraPos", cameraPos);
        drawModel(&backpackModel, shader);        
        
        glBindVertexArray(0);

        // Dont touch yet
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    deleteModel(&backpackModel);
    glDeleteProgram(shader);
    glDeleteProgram(shaderSkybox);
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
    static int i = 0;
    glViewport(0, 0, width, height);
    printf("%d.Changing size\n", i++);
}
