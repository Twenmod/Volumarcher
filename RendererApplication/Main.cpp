#include "pch.h"
#include "../MiniEngine/Core/GameCore.h"
#include "../MiniEngine/Core/GraphicsCore.h"
#include "../MiniEngine/Core/SystemTime.h"
#include "../MiniEngine/Core/TextRenderer.h"
#include "GameInput.h"
#include "../MiniEngine/Core/CommandContext.h"
#include "../MiniEngine/Core/RootSignature.h"
#include "../MiniEngine/Core/PipelineState.h"
#include "../MiniEngine/Core/BufferManager.h"

#include "Volumarcher.h"

using namespace GameCore;
using namespace Graphics;

class RendererApplication : public GameCore::IGameApp
{
public:
	RendererApplication()
	{
	}

	void Startup(void) override;

	void Cleanup(void) override;

	void Update(float deltaT) override;

	void RenderScene(void) override;

private:
	std::unique_ptr<Volumarcher::VolumetricContext> m_volumetricContext;
};


CREATE_APPLICATION(RendererApplication)

void RendererApplication::Startup(void)
{
	m_volumetricContext = std::make_unique<Volumarcher::VolumetricContext>();
}

void RendererApplication::Cleanup(void)
{
	// Free up resources in an orderly fashion
}

void RendererApplication::Update(float /*deltaT*/)
{
	ScopedTimer _prof(L"Update State");


	// Update something
}

void RendererApplication::RenderScene(void)
{
	GraphicsContext& gfxContext = GraphicsContext::Begin(L"Scene Render");

	gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	gfxContext.ClearColor(g_SceneColorBuffer);
	gfxContext.SetRenderTarget(g_SceneColorBuffer.GetRTV());
	gfxContext.SetViewportAndScissor(0, 0, g_SceneColorBuffer.GetWidth(), g_SceneColorBuffer.GetHeight());

	// Rendering something

	gfxContext.Finish();

	m_volumetricContext->Render(g_SceneColorBuffer);

}
