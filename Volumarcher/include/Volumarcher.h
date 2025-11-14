#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"

#include "glm/glm.hpp"

namespace Volumarcher
{
	class VolumetricContext
	{
	public:
		VolumetricContext();

		void Render(ColorBuffer _outputBuffer, glm::vec3 _camPos = glm::vec3{0.f}) const;
	private:
		ComputePSO m_computePSO;
		RootSignature m_rs;
	};
}
