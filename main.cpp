#include "mesh.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// constants
const static float kSizeSun = 1;
const static float kSizeEarth = 0.5;
const static float kSizeMoon = 0.25;
const static float kRadOrbitEarth = 10;
const static float kRadOrbitMoon = 2;
const static float T = 5.0f;

bool firstClick = true;
bool paused = false;
bool fpsMode = false;

double currentTime = 0.0;
double lastTime = 0.0;
double step = 0.001;

// define the string help
const std::string help = "Welcome to my Solar System Simulation!\n"
                         "Press H to display this help message anytime\n"
                          "Press P to pause the simulation\n"
                          "Press F to enable/disable FPS mode\n"
                          "Press T to enable wireframe mode\n"
                          "Press Y to disable wireframe mode\n"
                          "Press Z to move forward\n"
                          "Press S to move backward\n"
                          "Press Q to move left\n"
                          "Press D to move right\n"
                          "Press Space to move up\n"
                          "Press Left Control to move down\n"
                          "Press Left Shift to increase speed\n"
                          "Press Left Alt to decrease speed\n"
                          "Press ESC to exit the simulation\n";

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

GLuint g_earthTexID = 0;
GLuint g_moonTexID = 0;
GLuint g_sunTexID = 0;

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_ibo = 0;
GLuint g_colorVbo = 0;

// All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
std::vector<float> g_vertexPositions;
// All triangle indices packed in one array [v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle
std::vector<unsigned int> g_triangleIndices;
// All vertex colors packed in one array [r0, g0, b0, r1, g1, b1, ...]
std::vector<float> g_vertexColors;

std::shared_ptr<Mesh> sun = Mesh::genSphere(1.0f, 50);
std::shared_ptr<Mesh> earth = Mesh::genSphere(0.5f, 50);
std::shared_ptr<Mesh> moon = Mesh::genSphere(0.25f, 50);

Camera g_camera = Camera(1024, 768, glm::vec3(0.0f, 0.0f, 3.0f));

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  int width, height, numComponents;
  // Loading the image in CPU memory using stb_image
  unsigned char *data = stbi_load(
    filename.c_str(),
    &width, &height,
    &numComponents, // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
    0);

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  // Free useless CPU memory
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture

  return texID;
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
  g_camera.updateHeightWidth(width, height);
}

// Executed each time a key is entered.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_T) {
    std::cout << "Pressed T" << std::endl;
    std::cout << "Wireframe mode enabled, press Y to disable" << std::endl;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_Y) {
    std::cout << "Pressed Y" << std::endl;
    std::cout << "Wireframe mode disabled, press T to enable" << std::endl;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, true);
  }
  if(action == GLFW_PRESS && key == GLFW_KEY_H) {
    std::cout << help << std::endl;
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT) {
    if(key == GLFW_KEY_W) {
      g_camera.moveForward();
    }
    if(key == GLFW_KEY_S) {
      g_camera.moveBackward();
    }
    if(key == GLFW_KEY_A) {
      g_camera.moveLeft();
    }
    if(key == GLFW_KEY_D) {
      g_camera.moveRight();
    }
    if(key == GLFW_KEY_SPACE || key == GLFW_KEY_UP) {
      g_camera.moveUp();
    }
    if(key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_DOWN) {
      g_camera.moveDown();
    }
    if(key == GLFW_KEY_P) {
      paused = !paused;
      std::cout << "Pressed P" << std::endl;
      std::cout << "Scene Paused, press P to unpause" << std::endl;
    }
    if(key == GLFW_KEY_F){
      fpsMode = !fpsMode;
      std::cout << "Pressed F" << std::endl;
      if (fpsMode){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        std::cout << "FPS Mode enabled" << std::endl;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        std::cout << "FPS Mode disabled" << std::endl;
      }
    }
    if(key == GLFW_KEY_LEFT_SHIFT) {
      if (g_camera.getSpeed() < 0.5f){
        g_camera.changeSpeed(g_camera.getSpeed() + 0.01f);
        std::cout << "Speed increased to " << g_camera.getSpeed() << std::endl;
        std::cout << "Press left alt to decrease speed" << std::endl;
      }
      if (g_camera.getSpeed() > 0.5f){
          g_camera.changeSpeed(0.5f);
          std::cout << "Speed cannot be increased further" << std::endl;
        }
    } 
    if(key == GLFW_KEY_LEFT_ALT) {
      if (g_camera.getSpeed() > 0.02f){
        g_camera.changeSpeed(g_camera.getSpeed() - 0.01f);
        std::cout << "Speed decreased to " << g_camera.getSpeed() << std::endl;
        std::cout << "Press left shift to increase speed" << std::endl;
      }
      if (g_camera.getSpeed() < 0.02f){
        g_camera.changeSpeed(0.02f);
        std::cout << "Speed cannot be decreased further" << std::endl;
      }
    }
  }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  // center xpos and ypos
  if(fpsMode){
    g_camera.rotate(xpos, ypos);
    glfwSetCursorPos(window, g_camera.getLastMousePosition().x, g_camera.getLastMousePosition().y);
  }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
  glfwSetCursorPosCallback(g_window, mouseCallback);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprogram() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "vertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "fragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons

  glUseProgram(g_program);

  // Load the texture
  g_earthTexID = loadTextureFromFileToGPU("media/earth.jpg");
  g_moonTexID = loadTextureFromFileToGPU("media/moon.jpg");
  g_sunTexID = loadTextureFromFileToGPU("media/sun.jpg");

  // Pass the texture to the GPU program
  glUniform1i(glGetUniformLocation(g_program, "material.albedoTex"), 0);
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
}

void translateMeshToPosition(std::shared_ptr<Mesh> mesh, const glm::vec3 position) {
  glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
  mesh->setModelMatrix(modelMatrix);
}

void init() {
  initGLFW();
  initOpenGL();
  //initCPUgeometry();
  initGPUprogram();
  sun->init();
  earth->init();
  moon->init();
  //initGPUgeometry();
  initCamera();
  translateMeshToPosition(sun, glm::vec3(0.0f, 0.0f, 0.0f));
  translateMeshToPosition(earth, glm::vec3(kRadOrbitEarth, 0.0f, 0.0f));
  translateMeshToPosition(moon, glm::vec3(kRadOrbitEarth + kRadOrbitMoon, 0.0f, 0.0f));
}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// The main rendering call
void render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

  const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
  size_t width, height;
  glfwGetWindowSize(g_window, (int *)&width, (int *)&height);
  const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();
  const glm::vec3 camPosition = g_camera.getPosition();
  
  glUniform3f(glGetUniformLocation(g_program, "camPos"), camPosition[0], camPosition[1], camPosition[2]);
  glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix)); // compute the view matrix of the camera and pass it to the GPU program
  glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix)); // compute the projection matrix of the camera and pass it to the GPU program

  glUniform1i(glGetUniformLocation(g_program, "isSun"), 1);
  sun->render(g_program, g_sunTexID);

  glUniform1i(glGetUniformLocation(g_program, "isSun"), 0);
  earth->render(g_program, g_earthTexID);

  glUniform1i(glGetUniformLocation(g_program, "isSun"), 0);
  moon->render(g_program, g_moonTexID);
}

// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
  if (paused){
    return;
  }

  float rotationAngleEarth = (step / T) * 360;
  float orbitAngleEarth = (step / (2 * T)) * 360;
  float rotationAngleMoon = (step / (T / 2)) * 360;
  float orbitAngleMoon = rotationAngleMoon;
  
  glm::vec3 axis = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), glm::radians(23.5f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::vec3 tiltedAxis = glm::vec3(tilt * glm::vec4(axis, 0.0f));

  earth->orbitAndRotate(glm::vec3(0.0f, 0.0f, 0.0f), kRadOrbitEarth, tiltedAxis, orbitAngleEarth, tiltedAxis, rotationAngleEarth);

  moon->orbitAndRotate(earth->getPosition(), kRadOrbitMoon, axis, orbitAngleMoon, axis, rotationAngleMoon);

  step = step + 0.001;
}

int main(int argc, char ** argv) {
  std::cout << help << std::endl;
  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)
  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
