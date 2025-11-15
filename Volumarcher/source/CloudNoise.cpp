#include "CloudNoise.h"

#include <glm/ext/scalar_constants.hpp>

#include "../../MiniEngine/Core/Texture.h"
#include "external/libnoise/module/billow.h"


CloudNoise::CloudNoise(const glm::ivec3 _resolution) : VolumeNoise()
{
	std::vector<float> pixels = GenerateNoiseData(_resolution);
	m_billowNoiseTexture.Create3D(_resolution.x * sizeof(float), _resolution.x, _resolution.y, _resolution.z,
	                              DXGI_FORMAT_R32_FLOAT, pixels.data());
}

std::vector<float> CloudNoise::GenerateNoiseData(const glm::ivec3 _resolution)
{
	std::vector<float> data;
	data.reserve(_resolution.x * _resolution.y * _resolution.z);
	noise::module::Billow billowNoise;
	billowNoise.SetFrequency(3.17);
	for (int z = 0; z < _resolution.z; ++z)
	{
		float sZ = z / static_cast<float>(_resolution.z);
		for (int y = 0; y < _resolution.y; ++y)
		{
			float sY = y / static_cast<float>(_resolution.y);
			for (int x = 0; x < _resolution.x; ++x)
			{
				float sX = x / static_cast<float>(_resolution.x);

				data.push_back(abs(billowNoise.GetValue(sX, sY, sZ)));
			}
		}
	}

	return data;
}
