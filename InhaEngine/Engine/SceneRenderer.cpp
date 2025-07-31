#include "pch.h"
#include "SceneRenderer.h"
#include "Engine.h"
#include "SwapChainManager.h"
#include "RenderTargetGroup.h"
#include "Scene.h"
#include "Camera.h"
#include "ShadowMapCamera.h"
#include "CommandQueueManager.h"
#include "TableDescriptorHeapManager.h"
#include "Texture.h"

SceneRenderer::SceneRenderer() :
	Object(OBJECT_TYPE::SCENE_RENDERER)
{
}

void SceneRenderer::RenderScene(shared_ptr<Scene> scene) const
{
	assert(scene != nullptr);

	GlobalConstants globalConstants;	
	scene->FillGlobalData(globalConstants);
	ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsConstants(ROOT_PARAM::GLOBAL, &globalConstants, sizeof(globalConstants));

	RenderShadowMap(scene);
	PushGlobalTextures();	

	RenderForward(scene);
}

void SceneRenderer::PushGlobalTextures() const
{	
	ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::GLOBAL_TEXTURE);

	uint32 shadowMapCount = ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetDepthStencilBufferCount();
	for (uint32 i = 0; i < shadowMapCount; ++i)
	{
		shared_ptr<Texture> shadowMapTexture = ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetDepthStencilTexture(i);

		DescTableRegister reg(0);
		uint32 targetRegisterIdx = static_cast<uint32>(GLOBAL_TEXTURE_TABLE_REGISTER::t17) + i;
		reg.globalTextureRegister = static_cast<GLOBAL_TEXTURE_TABLE_REGISTER>(targetRegisterIdx);
		ENGINE->GetGraphicsDescHeapManager()->SetDesc(shadowMapTexture->GetSRVHeapHandle(), DESC_TABLE_TYPE::GLOBAL_TEXTURE, reg);
	}

	ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::GLOBAL_TEXTURE), GLOBAL_TEXTURE_TABLE_DESC_COUNT);
}

void SceneRenderer::RenderForward(shared_ptr<Scene> scene) const
{
	assert(scene);
	shared_ptr<SwapChainManager> swapChainManager = ENGINE->GetSwapChainManager();

	if (swapChainManager->IsMSAAEnabled())
	{
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::MSAA)->OMSetRenderTarget(0);
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::MSAA)->ClearRTVAndDSV(0);
	}
	else
	{
		uint32 backBufferIdx = ENGINE->GetSwapChainManager()->GetCurrentBackBufferIdx();
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTarget(backBufferIdx);
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRTVAndDSV(backBufferIdx);
	}	

	if (shared_ptr<Camera> mainCamera = scene->GetMainCamera())
	{
		ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::PASS);

		PerPassConstants passConstants;
		mainCamera->FillCameraData(passConstants);
		ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::PER_PASS)->PushGraphicsConstants(&passConstants, sizeof(passConstants));
		ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::PASS), PASS_TABLE_DESC_COUNT);

		mainCamera->SortGameObjects();
		mainCamera->Render();
	}
}

void SceneRenderer::RenderShadowMap(shared_ptr<Scene> scene) const
{
	assert(scene);
	for (shared_ptr<ShadowMapCamera> shadowMapCamera : scene->GetShadowMapCameras())
	{
		assert(shadowMapCamera != nullptr && "Can't process shadow map rendering as the shadow map camera is nullptr.");

		ComPtr<ID3D12GraphicsCommandList> cmdList = ENGINE->GetCommandQueueManager()->GetGraphicsCommandList();

		uint32 targetShadowMapIdx = shadowMapCamera->GetTargetShadowMapIdx();
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetDepthStencilTarget(targetShadowMapIdx);
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitResourceToDepthWrite(targetShadowMapIdx, cmdList);
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearDSV(targetShadowMapIdx);
	
		ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::PASS);

		PerPassConstants passConstants;
		shadowMapCamera->FillCameraData(passConstants);
		ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::PER_PASS)->PushGraphicsConstants(&passConstants, sizeof(passConstants));
		ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::PASS), PASS_TABLE_DESC_COUNT);

		shadowMapCamera->SortGameObjects();
		shadowMapCamera->RenderShadowMap();
	
		ENGINE->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitDepthWriteToResource(targetShadowMapIdx, cmdList);
	}
}
