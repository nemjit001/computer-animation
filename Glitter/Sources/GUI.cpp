#include "GUI.hpp"

GUI::GUI(GLFWwindow* pWindow, Camera& camera, SceneSettings& sceneSettings, Timer& timer, AssetLoader& loader)
    :
    p_window(pWindow),
    m_camera(camera),
    m_sceneSettings(sceneSettings),
    m_timer(timer),
    m_loader(loader),
    m_cameraMode("Camera Type: Normal Camera")
{
    //
}

void GUI::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void GUI::Render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    TimeData time = m_timer.GetData();
    auto& assets = m_loader.Get();
    std::string label;
    
    if (m_sceneSettings.active_asset)
        label = m_sceneSettings.active_asset->m_name;
    else
        label = std::string("Please select an asset");

    ImGui::Begin("Control Window");
    ImGui::Text("DeltaTime: %f", time.DeltaTime);
    ImGui::Text("FPS: %.2f", time.FPS);
    ImGui::Text("Animation Frame: %u", m_sceneSettings.animation_frame);
    ImGui::Text("Use SPACEBAR to enable/disable cursor!");
    ImGui::Text("Use P to start/pause the animation player!");
    ImGui::Text("Use R to reset the animation player!");
    ImGui::SliderFloat("Animation speed", &m_sceneSettings.anim_speed, 0.1f, 2.0f);

    if (ImGui::BeginCombo("Models", label.c_str()))
    {
        for (const auto& asset : assets)
        {
            bool selected = (m_sceneSettings.active_asset == asset.get());
            if (ImGui::Selectable(asset->m_name.c_str(), &selected))
                m_sceneSettings.active_asset = asset.get();

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::ColorEdit3("Base color", (float*)m_sceneSettings.base_color);
    ImGui::ColorEdit3("Manual light color", (float*)m_sceneSettings.light_color);
    ImGui::SliderFloat3("Light position", m_sceneSettings.light_position, -5.0f, 5.0f);
    ImGui::SliderFloat("Manual metallic", &m_sceneSettings.manual_metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Manual roughness", &m_sceneSettings.manual_roughness, 0.0f, 1.0f);
    ImGui::Checkbox("Toggle wireframe", &m_sceneSettings.wireframe_mode);
    ImGui::Text("%s", m_cameraMode.c_str());
    if (ImGui::Button("Switch Camera Modes"))
        GuiButtonCallback(GUI_BUTTON::CAMERA_MODE_TOGGLE);
    ImGui::Checkbox("Toggle DQS", &m_sceneSettings.dual_quat_skinning_flag);
    ImGui::Checkbox("Toggle Cubic interpolation", &m_sceneSettings.cubic_interpolation_flag);
    ImGui::Checkbox("Toggle Skybox", &m_sceneSettings.show_skybox);
    ImGui::Checkbox("Show bones", &m_sceneSettings.show_bones_flag);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::Cleanup()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool GUI::ShouldRenderBones() {
    return m_sceneSettings.show_bones_flag;
}

void GUI::GuiButtonCallback(GUI_BUTTON button)
{
    switch (button)
    {
    case GUI_BUTTON::MODEL_TOGGLE:
        break;
    case GUI_BUTTON::CAMERA_MODE_TOGGLE:
        m_camera.arcball_mode = !m_camera.arcball_mode;
        m_cameraMode = m_camera.arcball_mode ?
            std::string("Camera Type: Arcball Camera") : std::string("Camera Type: Normal Camera");
        break;
    default:
        break;
    }
}