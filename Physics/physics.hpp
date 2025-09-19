#pragma once

#include "glm/glm.hpp"

#include <vector>

struct Planet {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;

    glm::vec3 torque;
    glm::vec3 inertia;
    glm::vec3 angVel;
    glm::vec3 rot;

    float mass;
    float r;
};

class Physics {
private:
    std::vector<Planet> m_planets;
    float e = 0.8f; // Coefficient of restitution for collisions (elasticity a.k.a bounciness)

public:
    Physics();
    ~Physics();

    void addPlanet(const glm::vec3& pos, const glm::vec3& vel, float mass, float r);
    std::vector<Planet>* getPlanets() { return &m_planets; }
    void update(float dt);

private:
    void computeGravity(Planet& p1, Planet& p2);
    void integrate(Planet& p, float dt);
    void resolveCollision(Planet& p1, Planet& p2);
};