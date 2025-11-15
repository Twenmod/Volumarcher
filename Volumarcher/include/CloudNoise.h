#pragma once
#include <vector>

#include "VolumeNoise.h"
#include "../../MiniEngine/Core/Texture.h"
#include "glm/glm.hpp"

//Generator for billowy/wispy noise for cloud like volumes,
//uses LibNoise for generation
class CloudNoise final : public VolumeNoise
{
public:
	explicit CloudNoise(glm::ivec3 _resolution);

	[[nodiscard]] Texture GetBillowNoise() override { return m_billowNoiseTexture; }
private:
	Texture m_billowNoiseTexture;

	static std::vector<float> GenerateNoiseData(glm::ivec3 _resolution);
};
