#include "pch.h"

#include "Math.h"
#include <glm/gtx/vector_angle.hpp>

namespace MyEngine
{
    void WrapMinMax(int min, int max, int& numOut)
    {
        if (numOut < min)
        {
            numOut = max;
        }
        else if (numOut > max)
        {
            numOut = min;
        }
    }

    void WrapMinMax(float min, float max, float& numOut)
    {
        if (numOut < min)
        {
            numOut = max;
        }
        else if (numOut > max)
        {
            numOut = min;
        }
    }

    glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 bc = c - b;

        // Compute parametric position s for projection P' of P on AB,
        // P' = A + s*AB, s = snom/(snom+sdenom)
        float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

        // Compute parametric position t for projection P' of P on AC,
        // P' = A + t*AC, s = tnom/(tnom+tdenom)
        float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

        if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

        // Compute parametric position u for projection P' of P on BC,
        // P' = B + u*BC, u = unom/(unom+udenom)
        float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

        if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
        if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


        // P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
        glm::vec3 n = glm::cross(b - a, c - a);
        float vc = glm::dot(n, glm::cross(a - p, b - p));
        // If P outside AB and within feature region of AB,
        // return projection of P onto AB
        if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
            return a + snom / (snom + sdenom) * ab;

        // P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
        float va = glm::dot(n, glm::cross(b - p, c - p));
        // If P outside BC and within feature region of BC,
        // return projection of P onto BC
        if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
            return b + unom / (unom + udenom) * bc;

        // P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
        float vb = glm::dot(n, glm::cross(c - p, a - p));
        // If P outside CA and within feature region of CA,
        // return projection of P onto CA
        if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
            return a + tnom / (tnom + tdenom) * ac;

        // P must project inside face region. Compute Q using barycentric coordinates
        float u = va / (va + vb + vc);
        float v = vb / (va + vb + vc);
        float w = 1.0f - u - v; // = vc / (va + vb + vc)
        return u * a + v * b + w * c;
    }

    glm::vec3 ReplaceNaNWithValue(const glm::vec3& vec, const float& value)
    {
        glm::vec3 result;

        // For any coordinate if is nan replace with value
        for (int i = 0; i < 3; ++i) {
            if (std::isnan(vec[i]))
            {
                result[i] = value;
            }
            else 
            {
                result[i] = vec[i];
            }
        }

        return result;
    }

    float Ratio(int x, int y)
    {
        float ratio = 0.0f;
        if (y > 0)
        {
            ratio = x / (float)y;
        }

        return ratio;
    }

    float Angle(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3 axis)
    {
        glm::vec3 normalA = glm::normalize(vecA);
        glm::vec3 normalB = glm::normalize(vecB);

        return glm::orientedAngle(normalA, normalB, axis);
    }

    void CleanZeros(glm::vec3& value)
    {
        if ((value.x < MIN_FLOAT) && (value.x > -MIN_FLOAT))
        {
            value.x = 0.0f;
        }
        if ((value.y < MIN_FLOAT) && (value.y > -MIN_FLOAT))
        {
            value.y = 0.0f;
        }
        if ((value.z < MIN_FLOAT) && (value.z > -MIN_FLOAT))
        {
            value.z = 0.0f;
        }
    }

    void UpdateSpringConstraint(glm::vec3& positionA, glm::vec3& positionB,
                                const float& restLength, const float& springStregth,
                                bool moveB)
    {
        glm::vec3 delta = positionB - positionA;
        float deltalength = glm::length(delta);
        if (deltalength <= MIN_FLOAT)
        {
            return;
        }

        float diffA = (deltalength - restLength);
        float diff = diffA / deltalength;

        positionA += delta * 0.5f * diff * springStregth;
        CleanZeros(positionA);

        if (moveB)
        {
            positionB -= delta * 0.5f * diff * springStregth;
            CleanZeros(positionB);
        }
    }
}
