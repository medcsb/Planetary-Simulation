#include "physics.hpp"

Physics::Physics() {
}

Physics::~Physics() {
    m_planets.clear();
}

void Physics::addPlanet(const glm::vec3& pos, const glm::vec3& vel, float mass, float r) {
    Planet p;
    p.pos = pos;
    p.vel = vel;
    p.acc = glm::vec3(0.0f);

    p.torque = glm::vec3(0.0f);
    p.inertia = 2.0f/5.0f * mass * r * r * glm::vec3(1.0f); // I = 2/5 m r^2 for solid sphere
    p.angVel = glm::vec3(0.0f, 9.0f, 0.0f);
    p.rot = glm::vec3(0.0f);

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
            resolveCollision(m_planets[i], m_planets[j]);
            computeGravity(m_planets[i], m_planets[j]);
        }
    }

    // Update velocities and positions
    for (auto& planet : m_planets) {
        integrate(planet, dt);
    }
}

void Physics::computeGravity(Planet& p1, Planet& p2) {
    const float G = 6.67430e-6f; // Gravitational constant

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

void Physics::resolveCollision(Planet& p1, Planet& p2) {
    glm::vec3 dir = p2.pos - p1.pos;
    float dist = glm::length(dir);
    if (dist >= (p1.r + p2.r)) return;
    if (dist == 0.0f) dir = glm::vec3(1.0f, 0.0f, 0.0f);
    else dir = glm::normalize(dir);

    float rel_vel = glm::dot(p2.vel - p1.vel, dir);
    if (rel_vel > 0) return;

    float impulse = -(1 + e) * rel_vel / (1 / p1.mass + 1 / p2.mass);

    // penetration dcorrection
    float penetration = (p1.r + p2.r) - dist;
    if (penetration > 0.0f) {
        const float percent = 0.8f;
        const float slop = 0.01f;
        float correctionMag = std::max(penetration - slop, 0.0f) / (1/p1.mass + 1/p2.mass);
        glm::vec3 correction = correctionMag * percent * dir;

        p1.pos -= (1/p1.mass) * correction;
        p2.pos += (1/p2.mass) * correction;
    }

    p1.vel -= (impulse / p1.mass) * dir;
    p2.vel += (impulse / p2.mass) * dir;
}

void Physics::integrate(Planet& p, float dt) {
    p.vel += p.acc * dt;
    p.pos += p.vel * dt;

    p.angVel += p.torque / p.inertia * dt;
    p.rot += p.angVel * dt;
}
