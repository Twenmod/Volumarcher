#include "pch.h"
#include "../MiniEngine/Core/GameCore.h"
#include "../MiniEngine/Core/GraphicsCore.h"
#include "../MiniEngine/Core/SystemTime.h"
#include "../MiniEngine/Core/TextRenderer.h"
#include "GameInput.h"
#include "PostEffects.h"
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
	glm::vec3 m_camPos{0.f};
	glm::quat m_camRot{glm::identity<glm::quat>()};
	float m_camYaw{0.f};
	float m_camPitch{0.f};
	const float m_cameraSpeed{2.f};
	const float m_cameraRotSpeed{0.7f};
};


CREATE_APPLICATION(RendererApplication)

void RendererApplication::Startup(void)
{
	Volume volumes[VOLUME_AMOUNT] = {{float3(0, 0, 5), 2.f, 1.f}};
	m_volumetricContext = std::make_unique<Volumarcher::VolumetricContext>(volumes);
	PostEffects::BloomEnable = false;
}

void RendererApplication::Cleanup(void)
{
	// Free up resources in an orderly fashion
}

void RendererApplication::Update(const float _deltaTime)
{
	ScopedTimer _prof(L"Update State");


	//Rotation
	m_camYaw -= GameInput::GetAnalogInput(GameInput::kAnalogMouseX) * m_cameraRotSpeed;
	m_camPitch -= GameInput::GetAnalogInput(GameInput::kAnalogMouseY) * m_cameraRotSpeed;
	m_camPitch = glm::clamp(m_camPitch, -89.99f, 89.99f);
	m_camRot = glm::angleAxis(m_camYaw, glm::vec3(0, 1, 0)) * glm::angleAxis(m_camPitch, glm::vec3(1, 0, 0));

	//Movement
	glm::vec3 input{0.f};
	if (GameInput::IsPressed(GameInput::kKey_a)) input.x += 1;
	if (GameInput::IsPressed(GameInput::kKey_d)) input.x -= 1;
	if (GameInput::IsPressed(GameInput::kKey_w)) input.z += 1;
	if (GameInput::IsPressed(GameInput::kKey_s)) input.z -= 1;
	if (GameInput::IsPressed(GameInput::kKey_space)) input.y += 1;
	if (GameInput::IsPressed(GameInput::kKey_lcontrol)) input.y -= 1;
	if (glm::dot(input, input) > 0) input = normalize(input);
	m_camPos += (m_camRot * input) * m_cameraSpeed * _deltaTime;
}

void RendererApplication::RenderScene(void)
{
	m_volumetricContext->Render(g_SceneColorBuffer, m_camPos, m_camRot);
}
