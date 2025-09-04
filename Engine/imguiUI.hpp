#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Photon/Renderer.h"
#include "Shader.hpp"
#include "Scene.hpp"

#include <string>

struct FboSize {
    uint32_t width;
    uint32_t height;
    bool resized = false;
};

struct UI_Struct {
    FboSize* mainFboSize;
    ImTextureID* main_fbo_tex;
    Scene* scene;
    std::vector<Obj>* objects;
    std::vector<std::string>* objNames;
    std::vector<PBR_Renderable>* pbrRenderables;
    std::vector<Shader>* shaders;
    std::vector<Light>* lights;
    //Scene scene;
};

class ImguiUI {
private:
    GLFWwindow* m_window;
    ImGuiViewport* m_viewport;
    ImGuiIO* m_io;

    ImGuiDockNodeFlags m_dockspaceFlags;
    ImGuiWindowFlags m_windowFlags;

    uint32_t m_versionMajor;
    uint32_t m_versionMinor;

    bool m_onStartup = true;
    ImVec2 m_prevAvail;
    std::function<void(size_t)> m_onShaderReload;

    size_t m_selectedObjIdx = UINT32_MAX;

    double last_updated_time = 0;
    double current_time = 0;
    float fps = 0.0f;

public:
    ImguiUI();
    ~ImguiUI();

    void init(GLFWwindow* window , const std::string& glsl_version);

    void beginRender();
    void render(UI_Struct& ui_struct);
    void endRender();

    void setOnShaderReloadCallback(std::function<void(size_t)> callback) { m_onShaderReload = std::move(callback); }

    void cleanup();

private:
    void beginDockSpace();

    void renderScenePanel(ImTextureID sceneTexture, FboSize* mainFboSize);
    void renderInfoPanel(UI_Struct& ui_struct);
    void renderEditPanel(UI_Struct& ui_struct);
    void info();
    void settings();
    void sceneSettings(Scene* scene, std::vector<Light>* lights);
    void shaders(std::vector<Shader>* shaders);

    void transformEdit(Transform& transform);
    void pbrMaterialEdit(PBR_Renderable& renderable);
};