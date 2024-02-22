#include "pch.h"
#include "Random.h"

namespace MyEngine
{
    uint32_t Random::PCG_Hash(uint32_t input)
    {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    float Random::Float(uint32_t& seed)
    {
        seed = PCG_Hash(seed);
        return (float)seed / (float)UINT32_MAX;
    }

    float Random::Float(uint32_t& seed, float min, float max)
    {
        float randomFloat = Float(seed);

        // Scale the random float to fit the range
        return randomFloat * (max - min) + min;
    }

    int Random::Int(uint32_t& seed, int min, int max)
    {
        float randomFloat = Float(seed);

        // Scale the random float to fit the range
        return static_cast<int>(randomFloat * (max - min)) + min;
    }

    glm::vec3 Random::Vec3(uint32_t& seed, float min, float max)
    {
        return glm::vec3(
            Float(seed, min, max),
            Float(seed, min, max),
            Float(seed, min, max)
        );
    }

    glm::vec3 Random::Vec3(uint32_t& seed, glm::vec3 min, glm::vec3 max)
    {
        glm::vec3 vec = glm::vec3(
            Float(seed, min.x, max.x),
            Float(seed, min.y, max.y),
            Float(seed, min.z, max.z)
        );
        return vec;
    }

    glm::vec4 Random::Vec4(uint32_t& seed, float min, float max)
    {
        return glm::vec4(
            Float(seed, min, max),
            Float(seed, min, max),
            Float(seed, min, max),
            Float(seed, min, max)
        );
    }

    glm::vec4 Random::Vec4(uint32_t& seed, glm::vec4 min, glm::vec4 max)
    {
        return glm::vec4(
            Float(seed, min.w, max.w),
            Float(seed, min.x, max.x),
            Float(seed, min.y, max.y),
            Float(seed, min.z, max.z)
        );
    }

    glm::quat Random::Quat(uint32_t& seed, glm::quat min, glm::quat max)
    {
        return glm::quat(
            Float(seed, min.w, max.w),
            Float(seed, min.x, max.x),
            Float(seed, min.y, max.y),
            Float(seed, min.z, max.z)
        );
    }
}
