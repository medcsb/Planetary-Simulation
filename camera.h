#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

float rotationDistance = 5.0f;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
  public:
    Camera(int width, int height, glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f)) : Position(position), width(width), height(height) {}
    ~Camera() {delete this;}

    void resetLastMousePosition() {
        lastMousePosition = glm::vec2(width / 2, height / 2);
    }

    void updateLookAtPoint() {
        this->lookAtPoint = Position + Orientation * rotationDistance;
    }

    glm::mat4 computeViewMatrix() {
        return glm::lookAt(Position, Position + Orientation, Up);
    }

    glm::vec3 getPosition() {
        return Position;
    }

    glm::vec2 getLastMousePosition() {
        return lastMousePosition;
    }

    float getSpeed() {
        return speed;
    }

    glm::mat4 computeProjectionMatrix() {
        return glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    }

    void updateHeightWidth(int width, int height) {
        this->width = width;
        this->height = height;
        lastMousePosition = glm::vec2(width / 2, height / 2);
    }

    void moveForward() {
        Position += speed * Orientation;
    }

    void moveBackward() {
        Position -= speed * Orientation;
    }

    void moveRight() {
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    }

    void moveLeft() {
        Position -= speed * glm::normalize(glm::cross(Orientation, Up));
    }

    void moveUp() {
        Position += speed * Up;
    }

    void moveDown() {
        Position -= speed * Up;
    }

    void changeSpeed(float speed) {
        this->speed = speed;
    }

    void rotate(double xpos, double ypos) {
        // Calculate the mouse offset since the last frame
        double xoffset = xpos - lastMousePosition.x;
        double yoffset = lastMousePosition.y - ypos;

        // Scale offsets by sensitivity
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // Calculate the quaternion rotations around the Up and Right axes
        glm::quat qYaw = glm::angleAxis((float)-xoffset, Up); // Horizontal rotation
        glm::vec3 right = glm::normalize(glm::cross(Up, Orientation)); // Calculate the right axis
        glm::quat qPitch = glm::angleAxis((float)-yoffset, right); // Vertical rotation

        // Apply the rotations to the Orientation using quaternion multiplication
        glm::quat qRotation = qPitch * qYaw; // Combine rotations
        Orientation = glm::mat3_cast(qRotation) * Orientation; // Convert to mat3 and apply to Orientation

        // Update last mouse position
        lastMousePosition = glm::vec2(xpos, ypos);
    }


  private:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f);

    int width, height;

    float speed = 0.1f;
    float sensitivity = 0.001f;
    glm::vec2 lastMousePosition = glm::vec2(0.0f, 0.0f);
};

#endif // CAMERA_H