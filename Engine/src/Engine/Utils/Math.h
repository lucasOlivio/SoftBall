#pragma once

namespace MyEngine
{
    void WrapMinMax(int min, int max, int& numOut);

    void WrapMinMax(float min, float max, float& numOut);

    // From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
    // Chapter 5:
    glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

    glm::vec3 ReplaceNaNWithValue(const glm::vec3& vecIn, const float& value);

    // x/y or 0 if y = 0
    float Ratio(int x, int y);

    // Calculate the angle in quaternions between to vectors in reference to an axis
    float Angle(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3 axis);

    // Get only sign of number (1 or -1)
    template <typename T>
    int Sign(T value) { return (T(0) < value) - (value < T(0)); }

    // Set minimal float values to zero
    void CleanZeros(glm::vec3& value);

    // Constraint update for connected particles
    void UpdateSpringConstraint(glm::vec3& positionA, glm::vec3& positionB,
                                const float& restLength, const float& springStregth,
                                bool moveB = true);

    // Verlet integration to apply acceleration to particles
    void ApplyVerlet(glm::vec3& position, glm::vec3& oldPosition, 
                     const glm::vec3& acceleration, float deltaTime);
}
