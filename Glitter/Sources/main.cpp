// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Timer.hpp"
#include "GUI.hpp"
#include "Application.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <Mesh.hpp>
#include <Camera.hpp>

// Input Function Declarations
void processKeyboardInput(GLFWwindow* window);
void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos);
void mouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Input Tracking Globals
bool spacebar_down = false;
unsigned int mesh_index = 0;                                // Current Mesh
const unsigned int num_meshes = 5;                          // Total Number of Meshes

// Hacky way to store 5 meshes right now
Mesh* meshes[num_meshes];

// Create Render Settings Globals
static SceneSettings g_renderData =
{
    { -1.0f, 1.0f, -2.0f }, // default light pos
    { 1.0f, 1.0f, 0.0f },   // default base color
    { 0.5f, 1.0f, 0.0f },   // default light color
    0.0f,                   // default metallic color
    0.2f,                   // default roughness
    false,                  // default wireframe mode
    false                   // default bone visibility
};
// Create Camera Object
static Camera g_camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Create Timer object
static Timer g_timer;

// First Mouse Movement Hack
bool first_mouse_flag = true;
// Track Previous Camera Parameters
float lastX = (float)mWidth / 2.0;
float lastY = (float)mHeight / 2.0;

int main(int argc, char* argv[])
{
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glfwSetScrollCallback(mWindow, mouseScrollCallback);

    // Hide Cursor and Capture Mouse
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // create and link simple shader
    Shader defaultShader = Shader();
    defaultShader.init();

    defaultShader
        .registerShader("Shaders/lighting_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader_simple.frag", GL_FRAGMENT_SHADER)
        .link();

    // create and link texture shader
    Shader textureShader = Shader();
    textureShader.init();

    textureShader
        .registerShader("Shaders/lighting_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader.frag", GL_FRAGMENT_SHADER)
        .link();

    // Initialize our application and call its init function
    Application app = Application();
    app.init();

    // Initialize our GUI
    GUI gui = GUI(mWindow, g_camera, g_renderData, g_timer);
    gui.Init();

    Mesh mesh0("Assets/cube.obj", defaultShader);
    Mesh mesh1("Assets/suzanne.obj", defaultShader);
    Mesh mesh2("Assets/BASEmodel.fbx", textureShader);
    Mesh mesh3("Assets/test_model.fbx", textureShader);
    Mesh mesh4("Assets/wiggly.fbx", defaultShader);

    meshes[0] = &mesh0;
    meshes[1] = &mesh1;
    meshes[2] = &mesh2;
    meshes[3] = &mesh3;
    meshes[4] = &mesh4;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Tick the application state before the graphics update
        app.tick();

        g_timer.Tick();

        // Process Keyboard Input
        processKeyboardInput(mWindow);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Normal/Wireframe Rendering
        if (g_renderData.wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);        

        // Get View and Projection Matrics from Camera
        glm::mat4 view = g_camera.GetCurrentViewMatrix();
        glm::mat4 projection = g_camera.GetCurrentProjectionMatrix(mWidth, mHeight);

        // Render Mesh
        meshes[mesh_index]->Render(
            view,
            glm::mat4(1.0f),
            projection,
            g_camera.position,
            glm::vec3(g_renderData.light_position[0], g_renderData.light_position[1], g_renderData.light_position[2]),
            glm::vec3(g_renderData.base_color[0], g_renderData.base_color[1], g_renderData.base_color[2]),
            glm::vec3(g_renderData.light_color[0], g_renderData.light_color[1], g_renderData.light_color[2]),
            g_renderData.manual_metallic,
            g_renderData.manual_roughness
        );

        // Render GUI
        gui.Render();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    // Teardown application and GLFW
    app.shutdown();

    // Cleanup GUI
    gui.Cleanup();

    defaultShader.cleanup();
    textureShader.cleanup();

    glfwTerminate();

    return EXIT_SUCCESS;
}

// Process Keyboard Input
void processKeyboardInput(GLFWwindow* window)
{
    // Exit on ESC Key Press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Enable/Disable Camera
    if (spacebar_down && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        g_camera.enabled = !g_camera.enabled;
        
        // Enable/Disable Cursor
        if (g_camera.enabled)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        spacebar_down = false;
    }

    if (!spacebar_down && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        spacebar_down = true;

    // Ignore Keyboard Inputs for Camera Movement if arcball_mode == true
    if (g_camera.arcball_mode)
        return;

    TimeData time = g_timer.GetData();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.MoveCamera(FWD, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.MoveCamera(AFT, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.MoveCamera(LEFT, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.MoveCamera(RIGHT, time.DeltaTime);
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

    TimeData time = g_timer.GetData();
    if (g_camera.arcball_mode)
        g_camera.RotateArcballCamera(xoffset, yoffset, mWidth, mHeight, time.DeltaTime);
    else
        g_camera.RotateCamera(xoffset, yoffset);
}

void mouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    TimeData time = g_timer.GetData();
    g_camera.MoveArcballCamera(y_offset, time.DeltaTime);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
