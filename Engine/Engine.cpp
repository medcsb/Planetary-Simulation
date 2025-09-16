#include "Engine.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "utils/log.h"

#include <stdexcept>
#include <string>

#define ENGINE_MODE

Engine::Engine() {
    initWindow();
    initOpenGL();
    m_renderer.initFrameBuffer(m_mainFboSize.width, m_mainFboSize.height);
    m_ui.init(m_window.handle , std::to_string(GLSL_VERSION));
    m_ui.setOnShaderReloadCallback([this](size_t idx) {
        m_shaderPrograms[idx].reload();
        m_renderer.initPBRShaders(m_shaderPrograms[0].getProgramId());
    });
    Shader pbrShader{std::string(SHADER_DIR) + "pbr.vert", std::string(SHADER_DIR) + "pbr.frag"};
    Shader skyboxShader{std::string(SHADER_DIR) + "skybox.vert", std::string(SHADER_DIR) + "skybox.frag"};
    Shader lightShader{std::string(SHADER_DIR) + "light.vert", std::string(SHADER_DIR) + "light.frag"};
    pbrShader.init();
    skyboxShader.init();
    lightShader.init();
    m_shaderPrograms.push_back(pbrShader);
    m_shaderPrograms.push_back(skyboxShader);
    m_renderer.initPBRShaders(pbrShader.getProgramId());
    m_renderer.initCubeMapShaders(skyboxShader.getProgramId());
    m_renderer.setLightShaderProgram(lightShader.getProgramId());
    m_renderer.setPBRRenderables(m_scene.getPBRRenderables());
    m_renderer.setSkyBox(m_scene.getSkyBox());

    m_scene.initExample();

    std::cout <<sizeof(float) << std::endl;

    fillUIStruct();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

Engine::~Engine() {
    m_scene.cleanup();
    m_ui.cleanup();

    for (auto& shader : m_shaderPrograms) {
        shader.cleanup();
    }
    
    m_renderer.cleanup();
    
    glfwDestroyWindow(m_window.handle);
    glfwTerminate();
}

void Engine::run() {
    while(!glfwWindowShouldClose(m_window.handle)) {
        processInput(m_window.handle, ImGui::GetIO().DeltaTime);

        #ifdef ENGINE_MODE
        if (m_mainFboSize.resized) {
            m_renderer.initFrameBuffer(m_mainFboSize.width, m_mainFboSize.height);
            m_uiStruct.main_fbo_tex = (ImTextureID*)(intptr_t)m_renderer.getMainFrameColor();
            m_uiStruct.mainFboSize = &m_mainFboSize;
            m_mainFboSize.resized = false;
        }
        #endif

        
        #ifdef ENGINE_MODE
        m_camera.updateProjectionMatrix(m_mainFboSize.width, m_mainFboSize.height);
        #else
        m_camera.updateProjectionMatrix(m_window.width, m_window.height);
        #endif
        m_camera.updateViewMatrix();
        
        m_scene.setViewMatrix(m_camera.getViewMatrix());
        m_scene.setProjectionMatrix(m_camera.getProjectionMatrix());
        m_renderer.setRenderInfo(m_scene.getRenderInfo());

        #ifdef ENGINE_MODE
        m_renderer.renderToFbo(m_mainFboSize.width, m_mainFboSize.height);
        m_ui.beginRender();
        m_ui.render(m_uiStruct);
        m_ui.endRender(); 
        #else
        m_renderer.renderToScreen(m_window.width, m_window.height);
        #endif

        m_scene.update(0.016f);
        
        glfwSwapBuffers(m_window.handle);
        glfwPollEvents();
    }
}

void Engine::initWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to init glfw");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility

    m_window.handle = glfwCreateWindow(m_window.width, m_window.height, "PHOTON", nullptr, nullptr);
    if (!m_window.handle) throw std::runtime_error("Failed to create a glfw window");
    glfwSetWindowUserPointer(m_window.handle, this);
    glfwSetFramebufferSizeCallback(m_window.handle, framebuffer_size_callback);

    glfwMakeContextCurrent(m_window.handle);
}

void Engine::initOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load opengl functions");
    }
    glViewport(0, 0, m_window.width, m_window.height);
}

void Engine::fillUIStruct() {
    m_uiStruct.mainFboSize = &m_mainFboSize;
    m_uiStruct.main_fbo_tex = (ImTextureID*)(intptr_t)m_renderer.getMainFrameColor();
    m_uiStruct.scene = &m_scene;
    m_uiStruct.pbrRenderables = m_scene.getPBRRenderables();
    m_uiStruct.objNames = m_scene.getObjNames();
    m_uiStruct.shaders = &m_shaderPrograms;
    m_uiStruct.lights = &m_scene.getRenderInfo().lights;
}

void Engine::processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetTime() - m_inputTime > 0.016) {
        m_inputTime = glfwGetTime();
        m_cameraController.processMouseInput(window, deltaTime);
    }
    m_cameraController.processKeyboardInput(window, deltaTime);
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->resetWindowSize(width, height);
}
