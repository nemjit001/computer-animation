#pragma once

#include "Camera.hpp"
#include "Timer.hpp"
#include "AssetLoader.hpp"

#include <string>
#include <GLFW/glfw3.h>
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
    Asset* active_asset;
    int animation_frame;
};

/// <summary>
/// GUI wrapper that handles all imgui related calls
/// </summary>
class GUI
{
public:
    GUI(GLFWwindow* pWindow, Camera& camera, SceneSettings& sceneSettings, Timer& timer, AssetLoader& loader);

    /// <summary>
    /// Initialize our GUI wrapper
    /// </summary>
    void Init();

    /// <summary>
    /// Render our GUI with updated reference data
    /// </summary>
    void Render();

    /// <summary>
    /// Perform GUI cleanup
    /// </summary>
    void Cleanup();

    bool ShouldRenderBones();
private:
    /// <summary>
    /// The GUI callback is used to update our reference's state using the GUI_BUTTON enum
    /// </summary>
    void GuiButtonCallback(GUI_BUTTON button);

private:
    GLFWwindow* p_window;
    Camera& m_camera;
    SceneSettings& m_sceneSettings;
    Timer& m_timer;
    AssetLoader& m_loader;
    std::string m_cameraMode;
};
