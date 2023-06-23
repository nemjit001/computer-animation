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

#include <Mesh.hpp>
#include <Camera.hpp>
#include <Skybox.hpp>
#include <Timer.hpp>
#include <AnimationPlayer.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>

enum GUI_BUTTON {
    MODEL_SWITCH,
    MODEL_SELECT,
    CAMERA_MODE_SWITCH
};

// Input Function Declarations
void processKeyboardInput(GLFWwindow* window);
void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos);
void mouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void guiButtonCallback(GUI_BUTTON);

// Rendering Globals
Mesh* meshes[5];
//glm::vec3 light_position = glm::vec3(1.0f, 2.0f, 0.0f);
float light_position[3] = { -1.0f, 1.0f - 2.0f };
float manual_metallic = 0.0f;
float manual_roughness = 0.2f;

// GUI Globals
char* camera_mode_string = "Camera Type: Normal Camera";

// First Mouse Movement Hack
bool first_mouse_flag = true;

// Create Camera Object
Camera main_camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Animation Player
AnimationPlayer anim_player(0, nullptr);

// Input Tracking Globals
bool spacebar_down = false;
bool p_down = false;
bool wireframe_mode = false;                                // Wireframe Render Flag
bool show_bones_flag = false;                               // NOTHING YET!
bool show_skybox = true;                                    // Render Skybox Flag
bool dual_quat_skinning_flag = false;                       // Whether to perform skinning using DQS or Linear
unsigned int mesh_index = 2;                                // Current Mesh
const unsigned int num_meshes = 5;                          // Total Number of Meshes
unsigned int animation_index = 0;

// Timer
Timer timer;

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
    //fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Set Callbacks
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetCursorPosCallback(mWindow, mouseMovementCallback);
    glfwSetScrollCallback(mWindow, mouseScrollCallback);

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

    // create and link bone shader
    Shader boneShader = Shader();
    boneShader.init();

    boneShader
        .registerShader("Shaders/bone_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader.frag", GL_FRAGMENT_SHADER)
        .link();

    // create and link dual quaternion shader
    Shader dqShader = Shader();
    dqShader.init();

    dqShader
        .registerShader("Shaders/bone_dq_no_scale_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader.frag", GL_FRAGMENT_SHADER)
        .link();

    // create and link skybox shader
    Shader skyboxShader = Shader();
    skyboxShader.init();

    skyboxShader
        .registerShader("Shaders/skybox.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/skybox.frag", GL_FRAGMENT_SHADER)
        .link();

    defaultShader.use();

    // Initialize our application and call its init function
    Application app = Application();
    app.init();

    // Create Skybox
    Skybox skybox("Assets/Yokohama3/", skyboxShader);

    // Create Meshes
    /*Mesh mesh0("Assets/cube.obj", &defaultShader);
    Mesh mesh1("Assets/BASEmodel.fbx", &dqShader);
    Mesh mesh2("Assets/bob_lamp.fbx", &dqShader);
    Mesh mesh3("Assets/boy_animated.fbx", &boneShader);
    Mesh mesh4("Assets/wiggly.fbx", &dqShader);*/
    Mesh mesh0("Assets/cube.obj", &defaultShader);
    Mesh mesh1("Assets/BASEmodel.fbx", &boneShader);
    Mesh mesh2("Assets/bob_lamp.fbx", &boneShader);
    Mesh mesh3("Assets/boy_animated.fbx", &boneShader);
    Mesh mesh4("Assets/wiggly.fbx", &boneShader);

    meshes[0] = &mesh0;
    meshes[1] = &mesh1;
    meshes[2] = &mesh2;
    meshes[3] = &mesh3;
    meshes[4] = &mesh4;

    float base_color[] = { 1.0f, 1.0f, 0.0f };
    float light_color[] = { 0.5f, 1.0f, 0.0f };

    anim_player = AnimationPlayer(0, meshes[mesh_index]);

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Tick the application state before the graphics update
        app.tick();

        // Update Time
        timer.Tick();

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

        // Check whether mesh has animation and evaluate
        if (meshes[mesh_index]->HasAnimations())
        {
            // Check type of skinning
            if (dual_quat_skinning_flag)
            {
                meshes[mesh_index]->ChangeShader(&dqShader);
                //meshes[mesh_index]->AnimateDualQuat(animation_index);
                meshes[mesh_index]->AnimateBIDualQuat(anim_player.UpdateTime(timer.GetData().DeltaTime));
            }
            else
            {
                meshes[mesh_index]->ChangeShader(&boneShader);
                meshes[mesh_index]->AnimateBI(anim_player.UpdateTime(timer.GetData().DeltaTime));
            }
        }

        // Get View and Projection Matrics from Camera
        glm::mat4 view = main_camera.GetCurrentViewMatrix();
        glm::mat4 projection = main_camera.GetCurrentProjectionMatrix(mWidth, mHeight);

        // Render Mesh
        meshes[mesh_index]->Render(view, glm::mat4(1.0f), projection, main_camera.position, glm::vec3(light_position[0], light_position[1], light_position[2]), glm::vec3(base_color[0], base_color[1], base_color[2]), glm::vec3(light_color[0], light_color[1], light_color[2]), manual_metallic, manual_roughness);

        // Render Skybox
        if (show_skybox)
            skybox.Render(view, projection);

        // Render GUI
        ImGui::Begin("Control Window");
        ImGui::Text("DeltaTime: %f" , timer.GetData().DeltaTime);
        ImGui::Text("Animation Frame: %d", animation_index);
        ImGui::Text("Use SPACEBAR to enable/disable cursor!");
        ImGui::Text("Use P to start/pause the animation player!");
        if (ImGui::Button("Switch Model"))
            guiButtonCallback(MODEL_SWITCH);
        ImGui::ColorEdit3("Base color", (float*)base_color);
        ImGui::ColorEdit3("Manual light color", (float*)light_color);
        ImGui::SliderFloat3("Light position", light_position, -5.0f, 5.0f);
        ImGui::SliderFloat("Manual metallic", &manual_metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Manual roughness", &manual_roughness, 0.0f, 1.0f);
        ImGui::Checkbox("Toggle wireframe", &wireframe_mode);
        ImGui::Text(camera_mode_string);
        if (ImGui::Button("Switch Camera Modes"))
            guiButtonCallback(CAMERA_MODE_SWITCH);
        ImGui::Checkbox("Toggle DQS", &dual_quat_skinning_flag);
        ImGui::Checkbox("Toggle Skybox", &show_skybox);
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
    textureShader.cleanup();
    boneShader.cleanup();
    dqShader.cleanup();
    skyboxShader.cleanup();

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

    // Start/Pause Animation
    if (p_down && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        anim_player.is_playing = !anim_player.is_playing;

        p_down = false;
    }

    // Scrolling through animation
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && animation_index < meshes[mesh_index]->GetAnimationFrameNum() - 1)
        animation_index++;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && animation_index > 0)
        animation_index--;

    if (!spacebar_down && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        spacebar_down = true;

    if (!p_down && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        p_down = true;

    // Ignore Keyboard Inputs for Camera Movement if arcball_mode == true
    if (main_camera.arcball_mode)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        main_camera.MoveCamera(FWD, timer.GetData().DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        main_camera.MoveCamera(AFT, timer.GetData().DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        main_camera.MoveCamera(LEFT, timer.GetData().DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        main_camera.MoveCamera(RIGHT, timer.GetData().DeltaTime);
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

    if (main_camera.arcball_mode)
        main_camera.RotateArcballCamera(xoffset, yoffset, mWidth, mHeight, timer.GetData().DeltaTime);
    else
        main_camera.RotateCamera(xoffset, yoffset);
}

void mouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    main_camera.MoveArcballCamera(y_offset, timer.GetData().DeltaTime);
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
        animation_index = 0;            // Reset animation frame index
        if (mesh_index == num_meshes)
            mesh_index = 0;

        // Update AnimationPlayer
        if (meshes[mesh_index]->HasAnimations())
            anim_player.SetValues(0, meshes[mesh_index]);
    }

    else if (button == CAMERA_MODE_SWITCH)
    {
        main_camera.arcball_mode = !main_camera.arcball_mode;

        if (main_camera.arcball_mode)
            camera_mode_string = "Camera Type: Arcball Camera";
        else
            camera_mode_string = "Camera Type: Normal Camera";
    }
}