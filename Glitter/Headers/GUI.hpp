#pragma once

#include "Camera.hpp"
#include "Timer.hpp"

#include <string>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

/// <summary>
/// This enum stores different UI button callback types
/// </summary>
enum class GUI_BUTTON
{
    MODEL_TOGGLE,
    CAMERA_MODE_TOGGLE
};

/// <summary>
/// The SceneSettings struct stores light information,
/// default color information,
/// as well as different input or render modes
/// </summary>
struct SceneSettings
{
    float light_position[3];
    float base_color[3];
    float light_color[3];
    float manual_metallic;
    float manual_roughness;
    bool wireframe_mode;
    bool show_bones_flag;
};

/// <summary>
/// GUI wrapper that handles all imgui related calls
/// </summary>
class GUI
{
public:
    GUI(GLFWwindow* pWindow, Camera& camera, SceneSettings& sceneSettings, Timer& timer);

    void Init();

    void Render();

    void Cleanup();

private:
    void GuiButtonCallback(GUI_BUTTON button);

private:
    GLFWwindow* p_window;
    Camera& m_camera;
    SceneSettings& m_sceneSettings;
    Timer& m_timer;
    std::string m_cameraMode;
};
