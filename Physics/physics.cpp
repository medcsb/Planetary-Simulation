#include "physics.hpp"

Physics::Physics() {
}

Physics::~Physics() {
    m_planets.clear();
}

void Physics::addPlanet(const uint32_t id, const glm::vec3& pos, const glm::vec3& vel, float mass, float r) {
    Planet p;
    p.id = id;
    p.pos = pos;
    p.vel = vel;
    p.acc = glm::vec3(0.0f);
    p.mass = mass;
    p.r = r;
    m_planets.push_back(p);
}


void Physics::update(float dt) {
    // Reset accelerations
    for (auto& planet : m_planets) {
        planet.acc = glm::vec3(0.0f);
    }

    // Compute gravitational forces
    for (size_t i = 0; i < m_planets.size(); ++i) {
        for (size_t j = i + 1; j < m_planets.size(); ++j) {
            computeGravity(m_planets[i], m_planets[j]);
        }
    }

    // Update velocities and positions
    for (auto& planet : m_planets) {
        planet.vel += planet.acc * dt;
        planet.pos += planet.vel * dt;
    }
}

void Physics::computeGravity(Planet& p1, Planet& p2) {
    const float G = 6.67430e-7f; // Gravitational constant

    glm::vec3 direction = p2.pos - p1.pos;
    float distance = glm::length(direction);
    if (distance == 0.0f) return; // Prevent division by zero

    glm::vec3 forceDir = glm::normalize(direction);
    float forceMagnitude = (G * p1.mass * p2.mass) / (distance * distance);
    glm::vec3 force = forceDir * forceMagnitude;

    // Update accelerations
    p1.acc += force / p1.mass;
    p2.acc -= force / p2.mass; // Equal and opposite force
}
