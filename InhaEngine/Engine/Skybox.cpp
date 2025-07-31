#include "pch.h"
#include "Skybox.h"
#include "Engine.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "CommandQueueManager.h"

Skybox::Skybox() :
	Object(OBJECT_TYPE::SKYBOX),
	_mesh(nullptr),
	_material(nullptr)
{
}

void Skybox::RenderSkybox() const
{
	if (GetMesh() == nullptr || GetMaterial() == nullptr)
	{
		// TODO: Print warning message.
		return;
	}

	ComPtr<ID3D12GraphicsCommandList> commandList = ENGINE->GetCommandQueueManager()->GetGraphicsCommandList();
	commandList->IASetVertexBuffers(0, 1, &GetMesh()->GetVertexBufferView());
	commandList->IASetIndexBuffer(&GetMesh()->GetIndexBufferView());

	GetMaterial()->ApplyShader(commandList);
	GetMaterial()->PushData();
	commandList->DrawIndexedInstanced(GetMesh()->GetIndexTotalCount(), 1, 0, 0, 0);
}
