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
	int dataSize = _resolution.x * _resolution.y * _resolution.z;
	data.resize(dataSize);
	noise::module::Billow billowNoise;
	billowNoise.SetFrequency(4.17);

	for (int i = 0; i < dataSize; ++i)
	{
		int z = i / (_resolution.x * _resolution.y);
		int y = (i / _resolution.x) % _resolution.y;
		int x = i % _resolution.x;

		float sX = x / static_cast<float>(_resolution.x);
		float sY = y / static_cast<float>(_resolution.y);
		float sZ = z / static_cast<float>(_resolution.z);

		data[i] = abs(billowNoise.GetValue(sX, sY, sZ));
	}

	return data;
}
