#pragma once

#include "glad/glad.h"

#include "stb_image.h"

#include <string>
#include <vector>

#define HAS_ALBEDO_TEX     0x01 // bit 1
#define HAS_NORMAL_TEX     0x02 // bit 2
#define HAS_METALLIC_TEX   0x04 // bit 3
#define HAS_ROUGHNESS_TEX  0x08 // bit 4
#define HAS_AO_TEX         0x10 // bit 5

class Texture {
private:
    GLuint m_id = 0;
    GLint m_unit = 0;
public:
    Texture();
    ~Texture();

    GLuint getTextureId() const { return m_id; }
    GLint getTextureUnit() const { return m_unit; }
    void loadTexture(const std::string& path, GLint unit = 0, bool genMipMaps = true);
    void loadCubeMap(const std::vector<std::string>& paths, GLint unit = 0);
};

struct PBR_Texture {
    uint32_t id = UINT32_MAX;
    Texture albedo;
    Texture normal;
    Texture metallic;
    Texture roughness;
    Texture ao;
    int flags = 0;
};