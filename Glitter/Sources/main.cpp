// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Timer.hpp"
#include "AssetLoader.hpp"
#include "GUI.hpp"
#include <Skybox.hpp>
#include <AnimationPlayer.hpp>
#include "Application.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

// Input Function Declarations
void processKeyboardInput(GLFWwindow* window);
void mouseMovementCallback(GLFWwindow* window, double x_pos, double y_pos);
void mouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Create Render Settings Globals
static SceneSettings g_renderData =
{
    { -1.0f, 1.0f, -2.0f }, // default light pos
    { 1.0f, 1.0f, 0.0f },   // default base color
    { 1.0f, 1.0f, 1.0f },   // default light color
    0.0f,                   // default metallic color
    0.0f,                   // default roughness
    false,                  // default wireframe mode
    false,                  // default bone visibility
    true,                   // default skybox rendering
    false,                  // default dual_quaternion skinning
    false,                  // default cubic interpolation flag
    1.0f,                   // default animation speed
    nullptr,                // no active asset at first
    0                       // 0th frame is default for animation
};

// Create Camera Object
static Camera g_camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Create Timer object
static Timer g_timer;

// First Mouse Movement Hack
bool first_mouse_flag = true;

// Animation Player
AnimationPlayer anim_player(0, nullptr);

// Input Tracking Globals
bool spacebar_down = false;
bool p_down = false;

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

    // create and link bone shader
    Shader boneShader = Shader();
    boneShader.init();

    boneShader
        .registerShader("Shaders/bone_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader.frag", GL_FRAGMENT_SHADER)
        .link();

    // create and link dual quaternion (without scale) shader
    Shader dqShader = Shader();
    dqShader.init();

    dqShader
        .registerShader("Shaders/bone_dq_no_scale_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/lighting_shader.frag", GL_FRAGMENT_SHADER)
        .link();

    // create and link dual quaternion (with scale) shader
    Shader dqScaleShader = Shader();
    dqScaleShader.init();

    dqScaleShader
        .registerShader("Shaders/bone_dq_scale_shader.vert", GL_VERTEX_SHADER)
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
    Mesh::skeletonShader = Shader();
    Mesh::skeletonShader.init();
    
    Mesh::skeletonShader
        .registerShader("Shaders/skeleton_shader.vert", GL_VERTEX_SHADER)
        .registerShader("Shaders/skeleton_shader.frag", GL_FRAGMENT_SHADER)
        .link();
    

    // Initialize our dynamic asset loader and load fbx files from the asset folder
    AssetLoader assetLoader = AssetLoader();
    assetLoader.Load("Assets/*.fbx", boneShader);

    // Create Floor Mesh
    Mesh floor("Assets/ca_floor.fbx", &textureShader);

    // Initialize our GUI
    GUI gui = GUI(mWindow, g_camera, g_renderData, g_timer, assetLoader);
    gui.Init();

    // Set Animation Player
    //anim_player = AnimationPlayer(0, g_renderData.active_asset->m_mesh.get());

    // Previously selected mesh
    Mesh* previous_mesh = nullptr;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Update Timer
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
  
        GLuint texture_diffuseID = 0;
        GLuint texture_normalID = 1;
        GLuint texture_specularID = 2;
      
        // Render floor
        floor.Render(
            view,
            glm::mat4(1.0f),
            projection,
            g_camera.position,
            glm::vec3(g_renderData.light_position[0], g_renderData.light_position[1], g_renderData.light_position[2]),
            glm::vec3(g_renderData.base_color[0], g_renderData.base_color[1], g_renderData.base_color[2]),
            glm::vec3(g_renderData.light_color[0], g_renderData.light_color[1], g_renderData.light_color[2]),
            g_renderData.manual_metallic,
            g_renderData.manual_roughness,
            texture_diffuseID,
            texture_normalID,
            texture_specularID
        );

        // Render Mesh
        if (g_renderData.active_asset)
        {
            Mesh* pActiveMesh = g_renderData.active_asset->m_mesh.get();

            // Check if mesh selection has changed
            if (pActiveMesh != previous_mesh)
            {
                // Update AnimationPlayer
                if (pActiveMesh->HasAnimations())
                    anim_player.SetValues(0, pActiveMesh);

                previous_mesh = pActiveMesh;
            }

            // Check whether mesh has animation and evaluate
            if (pActiveMesh->HasAnimations())
            {
                std::vector<glm::vec3> boneVertices = std::vector<glm::vec3>();

                // Check type of skinning
                if (g_renderData.dual_quat_skinning_flag)
                {
                    pActiveMesh->ChangeShader(&dqShader);
                    if (g_renderData.cubic_interpolation_flag)
                        pActiveMesh->AnimateCIDualQuat(anim_player.UpdateTime(g_timer.GetData().DeltaTime, g_renderData.anim_speed), &boneVertices);
                    else
                        pActiveMesh->AnimateLIDualQuat(anim_player.UpdateTime(g_timer.GetData().DeltaTime, g_renderData.anim_speed), &boneVertices);
                }
                else
                {
                    pActiveMesh->ChangeShader(&boneShader);
                    if (g_renderData.cubic_interpolation_flag)
                        pActiveMesh->AnimateCI(anim_player.UpdateTime(g_timer.GetData().DeltaTime, g_renderData.anim_speed), &boneVertices);
                    else
                        pActiveMesh->AnimateLI(anim_player.UpdateTime(g_timer.GetData().DeltaTime, g_renderData.anim_speed), &boneVertices);
                }

                //pActiveMesh->Animate(g_renderData.animation_frame, &boneVertices);

                Mesh::UpdateSkeletonVertices(boneVertices);
            }

            pActiveMesh->Render(
                view,
                glm::mat4(1.0f),
                projection,
                g_camera.position,
                glm::vec3(g_renderData.light_position[0], g_renderData.light_position[1], g_renderData.light_position[2]),
                glm::vec3(g_renderData.base_color[0], g_renderData.base_color[1], g_renderData.base_color[2]),
                glm::vec3(g_renderData.light_color[0], g_renderData.light_color[1], g_renderData.light_color[2]),
                g_renderData.manual_metallic,
                g_renderData.manual_roughness,
                texture_diffuseID,
                texture_normalID,
                texture_specularID
            );

            if (gui.ShouldRenderBones()) {
                // Disable depth testing so that the skeleton rendering is always on top
                glDisable(GL_DEPTH_TEST);

                Mesh::RenderBones(
                    view,
                    glm::mat4(1.0f),
                    projection
                );

                // Re-enable depth testing
                glEnable(GL_DEPTH_TEST);
            }
        }

        // Render Skybox
        if (g_renderData.show_skybox)
            skybox.Render(view, projection);
        
        // Render GUI
        gui.Render();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    // Cleanup GUI
    gui.Cleanup();

    defaultShader.cleanup();
    textureShader.cleanup();
    boneShader.cleanup();
    dqShader.cleanup();
    skyboxShader.cleanup();

    Mesh::skeletonShader.cleanup();

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

    // Start/Pause Animation
    if (p_down && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        anim_player.is_playing = !anim_player.is_playing;

        p_down = false;
    }

    // Scrolling through animation
    if (g_renderData.active_asset)
    {
        Mesh* pActiveMesh = g_renderData.active_asset->m_mesh.get();
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && g_renderData.animation_frame < pActiveMesh->GetAnimationFrameNum() - 1)
            g_renderData.animation_frame++;
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && g_renderData.animation_frame > 0)
            g_renderData.animation_frame--;
    }

    if (!spacebar_down && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        spacebar_down = true;

    if (!p_down && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        p_down = true;

    // Ignore Keyboard Inputs for Camera Movement if arcball_mode == true
    if (g_camera.arcball_mode)
        return;

    TimeData time = g_timer.GetData();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.MoveCamera(FWD, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.MoveCamera(AFT, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        g_camera.MoveCamera(UPWARD, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        g_camera.MoveCamera(DOWNWARD, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.MoveCamera(LEFT, time.DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.MoveCamera(RIGHT, time.DeltaTime);
}

void mouseMovementCallback(GLFWwindow * window, double x_pos, double y_pos)
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