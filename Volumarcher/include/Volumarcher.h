#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"


namespace Volumarcher
{
	class VolumetricContext
	{
	public:
		VolumetricContext();

		void Render(ColorBuffer _outputBuffer) const;
	private:
		ComputePSO m_computePSO;
		RootSignature m_rs;
	};
}
