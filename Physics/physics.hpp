#pragma once

#include "glm/glm.hpp"

#include <vector>

struct Planet {
    uint32_t id;
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    float mass;
    float r;
};

class Physics {
private:
    std::vector<Planet> m_planets;

public:
    Physics();
    ~Physics();

    void addPlanet(const uint32_t id, const glm::vec3& pos, const glm::vec3& vel, float mass, float r);
    std::vector<Planet>* getPlanets() { return &m_planets; }
    void update(float dt);

private:
    void computeGravity(Planet& p1, Planet& p2);
};