#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Vertex //room layout
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

void keyboardInput(GLFWwindow* window, Camera& camera, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

float lastX = 512.0f, lastY = 384.0f; // Mouse start at screen center
bool firstMouse = true;
bool leftButtonHeld = false;

GLuint roomVAO, roomVBO;

std::vector<Vertex> cubeVertices = {
    // Back wall (-Z)
    {{-10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {0.0f, 0.0f,  1.0f}},
    {{ 10.0f, -10.0f, -10.0f}, {1.0f, 0.0f}, {0.0f, 0.0f,  1.0f}},
    {{ 10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},
    {{ 10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},
    {{-10.0f,  10.0f, -10.0f}, {0.0f, 1.0f}, {0.0f, 0.0f,  1.0f}},
    {{-10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {0.0f, 0.0f,  1.0f}},

    // Front wall (+Z)
    {{-10.0f, -10.0f,  10.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 10.0f, -10.0f,  10.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 10.0f,  10.0f,  10.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 10.0f,  10.0f,  10.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-10.0f,  10.0f,  10.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-10.0f, -10.0f,  10.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

    // Left wall (-X)
    {{-10.0f, -10.0f,  10.0f}, {0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}},
    {{-10.0f, -10.0f, -10.0f}, {1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}},
    {{-10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}},
    {{-10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}},
    {{-10.0f,  10.0f,  10.0f}, {0.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}},
    {{-10.0f, -10.0f,  10.0f}, {0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}},

    // Right wall (+X)
    {{ 10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{ 10.0f, -10.0f,  10.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{ 10.0f,  10.0f,  10.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{ 10.0f,  10.0f,  10.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{ 10.0f,  10.0f, -10.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{ 10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

    // Bottom wall (-Y)
    {{-10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 10.0f, -10.0f, -10.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 10.0f, -10.0f,  10.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 10.0f, -10.0f,  10.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-10.0f, -10.0f,  10.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-10.0f, -10.0f, -10.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

    // Top wall (+Y)
    {{-10.0f,  10.0f,  10.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 10.0f,  10.0f,  10.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 10.0f,  10.0f, -10.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{-10.0f,  10.0f, -10.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{-10.0f,  10.0f,  10.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
};

// Function prototypes 
void keyboardInput(GLFWwindow* window, Camera& camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard('W', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard('S', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard('A', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard('D', deltaTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            leftButtonHeld = true;
        else if (action == GLFW_RELEASE)
            leftButtonHeld = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!leftButtonHeld)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // reversed Y

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}



int main( void )
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "Computer Graphics Coursework", NULL, NULL);
    
    if( window == NULL ){
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    GLuint shaderProgram = LoadShaders("vertexShader.glsl", "fragmentShader.glsl"); //shaders

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Setup the cube VAO/VBO
    glGenVertexArrays(1, &roomVAO);
    glGenBuffers(1, &roomVBO);

    glBindVertexArray(roomVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Vertex), &cubeVertices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // UVs
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    // Normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);

    GLuint whiteTex;
    //checkerboard texture
    const int texWidth = 64;
    const int texHeight = 64;
    unsigned char checkerData[texWidth * texHeight * 3];

    for (int y = 0; y < texHeight; ++y)
    {
        for (int x = 0; x < texWidth; ++x)
        {
            int checker = ((x / 8) % 2) ^ ((y / 8) % 2);  // 8x8 checker squares
            int index = (y * texWidth + x) * 3;
            checkerData[index + 0] = checker ? 200 : 100; // R
            checkerData[index + 1] = checker ? 100 : 50;  // G
            checkerData[index + 2] = checker ? 100 : 50;  // B
        }
    }

    glGenTextures(1, &whiteTex);
    glBindTexture(GL_TEXTURE_2D, whiteTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);


    // -------------------------------------------------------------------------
    // End of window creation
    // =========================================================================
    
    // Ensure we can capture keyboard inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f; // time of last frame


    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get inputs
        keyboardInput(window, camera, deltaTime);

        // Clear the window
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glUseProgram(shaderProgram);

        // Send matrices

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Lighting and material
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 10.0f, 8.0f, 10.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera.Position[0]);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "ka"), 0.2f);
        glUniform1f(glGetUniformLocation(shaderProgram, "kd"), 0.7f);
        glUniform1f(glGetUniformLocation(shaderProgram, "ks"), 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "Ns"), 32.0f);

        // Texture bind
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, whiteTex);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);

        // Draw
        glBindVertexArray(roomVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(cubeVertices.size()));
        glBindVertexArray(0);

        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &roomVAO);
    glDeleteBuffers(1, &roomVBO);
    glDeleteTextures(1, &whiteTex);
    glDeleteProgram(shaderProgram);


    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

