#include "GUI.hpp"

GUI::GUI(GLFWwindow* pWindow, Camera& camera, SceneSettings& sceneSettings)
    :
    p_window(pWindow),
    m_camera(camera),
    m_sceneSettings(sceneSettings),
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

    ImGui::Begin("Control Window");
    //ImGui::Text("DeltaTime: %f", deltaTime);
    ImGui::Text("Use SPACEBAR to enable/disable cursor!");
    if (ImGui::Button("Switch Model"))
        GuiButtonCallback(GUI_BUTTON::MODEL_TOGGLE);
    ImGui::ColorEdit3("Base color", (float*)m_sceneSettings.base_color);
    ImGui::ColorEdit3("Manual light color", (float*)m_sceneSettings.light_color);
    ImGui::SliderFloat3("Light position", m_sceneSettings.light_position, -5.0f, 5.0f);
    ImGui::SliderFloat("Manual metallic", &m_sceneSettings.manual_metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Manual roughness", &m_sceneSettings.manual_roughness, 0.0f, 1.0f);
    ImGui::Checkbox("Toggle wireframe", &m_sceneSettings.wireframe_mode);
    ImGui::Text(m_cameraMode.c_str());
    if (ImGui::Button("Switch Camera Modes"))
        GuiButtonCallback(GUI_BUTTON::CAMERA_MODE_TOGGLE);
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
