#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"
#include "../shaders/shaderCommon.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Volumarcher
{
	class VolumetricContext
	{
	public:
		VolumetricContext() = delete;

		explicit VolumetricContext(Volume _volumes[VOLUME_AMOUNT]);

		void SetVolumes(Volume _volumes[VOLUME_AMOUNT]);

		void Render(ColorBuffer _outputBuffer, glm::vec3 _camPos = glm::vec3{0.f},
		            glm::quat _camRot = glm::identity<glm::quat>()) const;

	private:
		ComputePSO m_computePSO;
		RootSignature m_rs;
		StructuredBuffer m_volumeBuffer;
	};
}
