#include "Graphics/shader.h"

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

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

int main() {
    //glfw config
    if(!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, 0);

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
    SHADER shader, shaderSingleColor, shaderFramebuffer;
    s_set(&shader, "VertexShader.glsl", "FragShader.glsl");
    s_set(&shaderSingleColor, "VertexShader.glsl", "FragShaderSingleColor.glsl");
    s_set(&shaderFramebuffer, "VertexFramebuffer.glsl", "FragFramebuffer.glsl"); 
     
    //=======================================================================
    // MODEL LOADING
     
    Model cubeModel, backpackModel;
    loadModel(&backpackModel, "Assets/Backpack/backpack.obj", "Assets/Backpack/"); 
    loadModel(&cubeModel, "Assets/cube.obj", "Assets/"); 
/* 
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
 */

    objectsInit();
    //=======================================================================
    // VBO & VAO & EBO & RBO
    
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 

    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); 
   // glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int  rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //glBindRenderbuff(GL_RENDERBUFFER, 0);

    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: FRAMEBUFFER WASNT COMPLETED\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    unsigned int transparentTexture = loadTexture("Assets/images/blending_window.png");
    unsigned int woodenFloorTexture = loadTexture("Assets/images/container.jpg");
    //=======================================================================
    //
    glfwSetKeyCallback(window, keycallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mousecallback);

    vec3 positions[] =
        {
            {-1, 1, -5},
            {2, 3, -4},
            {0,0,-3},
            {4, -2, 0},
        };
    float timer = 0;
    while (!glfwWindowShouldClose(window)) {

        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();
        procesInput(window);
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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

        vec3 color = {0,1,1};

        glUseProgram(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        
        s_setMatrix4fv(shader, "projection", 1, GL_FALSE, projection[0]);
        s_setMatrix4fv(shader, "view", 1, GL_FALSE, view[0]);
        for (int i = 0; i < sizeof(positions)/sizeof(vec3); ++i) {

            glm_mat4_identity(model);
            glm_translate(model, positions[i]);
            s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glm_mat4_identity(model);
        s_setMatrix4fv(shader, "model", 1, GL_FALSE, model[0]);
        drawModel(&backpackModel, shader);        
        glBindVertexArray(0);
        //drawModel(&cubeModel, shader);
        // ----------------------------------------------------
      
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(.4, .4, .3, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderFramebuffer);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
       
        // Dont touch yet
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    deleteModel(&cubeModel);
    glDeleteProgram(shader);
    glDeleteProgram(shaderSingleColor);
    glDeleteProgram(shaderFramebuffer);
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
