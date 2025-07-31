#pragma once

#include "Component.h"
#include "SkeletalMesh.h"

class Material;

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer() = default;

public:
	void Render();
	void RenderShadowMap();

private:
	virtual void Start() override;
	virtual void Update() override;

public:
	void LoadMesh(const wstring& path);

	inline shared_ptr<Mesh> GetMesh() { return _mesh; }
	inline void SetMesh(shared_ptr<Mesh> newMesh) { _mesh = newMesh; }

	// If mesh's object type is SKELETAL_MESH, it will cast it to SkeletalMesh and return.
	// If not, returns nullptr.
	inline shared_ptr<SkeletalMesh> GetSkeletalMesh()
	{
		if (GetMesh()->GetObjectType() != OBJECT_TYPE::SKELETAL_MESH) return nullptr;
		return static_pointer_cast<SkeletalMesh>(GetMesh());
	}

private:
	shared_ptr<Mesh> _mesh;
};

