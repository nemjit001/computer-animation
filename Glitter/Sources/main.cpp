// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Application.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include "Mesh.hpp"
#include <Camera.hpp>

// Input Function Declarations
void processKeyboardInput(GLFWwindow* window);
void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Time Keeping Globals
float prev_frame_time = 0.0f;
float deltaTime = 0.0f;

// First Mouse Movement Hack
bool first_mouse_flag = true;

// Create Camera Object
Camera main_camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Track Previous Camera Parameters
float lastX = (float)mWidth / 2.0;
float lastY = (float)mHeight / 2.0;

int main(int argc, char * argv[])
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, mAppName, nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Set Callbacks
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetCursorPosCallback(mWindow, mouseMovementCallback);

    // Hide Cursor and Capture Mouse
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // create and link shader program using our vert & frag shaders
    Shader defaultShader = Shader();
    defaultShader.init();

    defaultShader
        .registerShader("Shaders/shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/shader.frag", GL_FRAGMENT_SHADER)
        .link();

    defaultShader.use();

    // Initialize our application and call its init function
    Application app = Application();
    app.init();

    Mesh exampleMesh;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Tick the application state before the graphics update
        app.tick();

        // Update Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - prev_frame_time;
        prev_frame_time = currentFrame;

        // Process Keyboard Input
        processKeyboardInput(mWindow);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        defaultShader.use();

        // Get View and Projection Matrics from Camera
        glm::mat4 view = main_camera.GetCurrentViewMatrix();
        glm::mat4 projection = main_camera.GetCurrentProjectionMatrix(mWidth, mHeight);

        // Set Uniforms
        defaultShader.setMat4("viewMatrix", view);
        defaultShader.setMat4("modelMatrix", glm::mat4(1.0f));
        defaultShader.setMat4("projectionMatrix", projection);

        // Render Mesh
        exampleMesh.Render();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    
    // Teardown application and GLFW
    app.shutdown();

    defaultShader.cleanup();
    glfwTerminate();

    return EXIT_SUCCESS;
}

// Process Keyboard Input
void processKeyboardInput(GLFWwindow* window)
{
    // Exit on ESC Key Press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        main_camera.MoveCamera(FWD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        main_camera.MoveCamera(AFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        main_camera.MoveCamera(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        main_camera.MoveCamera(RIGHT, deltaTime);
}

void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos)
{
    float xpos = static_cast<float>(x_pos);
    float ypos = static_cast<float>(y_pos);

    if (first_mouse_flag)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse_flag = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    main_camera.RotateCamera(xoffset, yoffset);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}