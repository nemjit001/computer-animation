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
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>

enum GUI_BUTTON {
    MODEL_SWITCH,
    MODEL_SELECT
};

// Input Function Declarations
void processKeyboardInput(GLFWwindow* window);
void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void guiButtonCallback(GUI_BUTTON);

// Rendering Globals
Mesh* meshes[2];

// Time Keeping Globals
float prev_frame_time = 0.0f;
float deltaTime = 0.0f;

// First Mouse Movement Hack
bool first_mouse_flag = true;

// Create Camera Object
Camera main_camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Input Tracking Globals
bool spacebar_down = false;
bool wireframe_mode = false;                                // Wireframe Render Flag
bool show_bones_flag = false;                               // NOTHING YET!
unsigned int mesh_index = 0;                                // Current Mesh
const unsigned int num_meshes = 2;                          // Total Number of Meshes

// Track Previous Camera Parameters
float lastX = (float)mWidth / 2.0;
float lastY = (float)mHeight / 2.0;

int main(int argc, char* argv[])
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
    //fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Set Callbacks
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetCursorPosCallback(mWindow, mouseMovementCallback);

    // Hide Cursor and Capture Mouse
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup GUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

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

    Mesh mesh0("Assets/cube.obj");
    Mesh mesh1("Assets/suzanne.obj");

    meshes[0] = &mesh0;
    meshes[1] = &mesh1;

    float base_color[] = { 1.0f, 1.0f, 0.0f };

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

        // Normal/Wireframe Rendering
        if (wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // GUI Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        defaultShader.use();

        // Get View and Projection Matrics from Camera
        glm::mat4 view = main_camera.GetCurrentViewMatrix();
        glm::mat4 projection = main_camera.GetCurrentProjectionMatrix(mWidth, mHeight);

        // Set Uniforms
        defaultShader.setMat4("viewMatrix", view);
        defaultShader.setMat4("modelMatrix", glm::mat4(1.0f));
        defaultShader.setMat4("projectionMatrix", projection);
        defaultShader.setVec3("baseColor", glm::vec3(base_color[0], base_color[1], base_color[2]));

        // Render Mesh
        meshes[mesh_index]->Render();

        // Render GUI
        ImGui::Begin("Control Window");
        ImGui::Text("DeltaTime: %f" , deltaTime);
        ImGui::Text("Use SPACEBAR to enable/disable cursor!");
        if (ImGui::Button("Switch Model"))
            guiButtonCallback(MODEL_SWITCH);
        ImGui::ColorEdit3("Base color", (float*)base_color);
        ImGui::Checkbox("Toggle wireframe", &wireframe_mode);
        ImGui::Checkbox("Show bones", &show_bones_flag);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    // Teardown application and GLFW
    app.shutdown();

    defaultShader.cleanup();

    // Clean up GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
        main_camera.enabled = !main_camera.enabled;
        
        // Enable/Disable Cursor
        if (main_camera.enabled)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        spacebar_down = false;
    }

    if (!spacebar_down && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        spacebar_down = true;

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

void guiButtonCallback(GUI_BUTTON button)
{
    if (button == MODEL_SWITCH)
    {
        mesh_index++;
        if (mesh_index == num_meshes)
            mesh_index = 0;
    }
}