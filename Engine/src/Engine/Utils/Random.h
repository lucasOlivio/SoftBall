#pragma once

namespace MyEngine
{
	class Random
	{
	public:
		Random() {};
		~Random() {};

		// Fast random unsigned int generator
		// Ref: https://www.youtube.com/watch?v=5_RAHZQCPjE
		static uint32_t PCG_Hash(uint32_t input);

		// Uses PCG hash to generate a random float between 0 and 1
		static float Float(uint32_t& seed);

		// Uses Float to generate random float between min and max
		static float Float(uint32_t& seed, float min, float max);

		// Uses Float to generate random integer between min and max
		static int Int(uint32_t& seed, int min, int max);

		// Uses Float to generate random x,y,z values between min and max
		static glm::vec3 Vec3(uint32_t& seed, float min, float max);
		static glm::vec3 Vec3(uint32_t& seed, glm::vec3 min, glm::vec3 max);

		// Uses Float to generate random w,x,y,z values between min and max
		static glm::vec4 Vec4(uint32_t& seed, float min, float max);
		static glm::vec4 Vec4(uint32_t& seed, glm::vec4 min, glm::vec4 max);
		static glm::quat Quat(uint32_t& seed, glm::quat min, glm::quat max);
	};
}
