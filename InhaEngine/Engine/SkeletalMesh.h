#pragma once

#include "Mesh.h"

class Skeleton;
class AnimationInstance;

struct SkinnedVertex
{
	SkinnedVertex() = default;

	SkinnedVertex(const Vec3& position, const Vec2& uv, const Vec3& normal, const Vec3& tangent) :
		position(position), uv(uv), normal(normal), tangent(tangent), weights(Vec3(0.f))
	{
	}

	SkinnedVertex(const Vec3& position, const Vec2& uv, const Vec3& normal, const Vec3& tangent, const VecInt4& linkedBonesId, const Vec4& weights) :
		position(position), uv(uv), normal(normal), tangent(tangent), weights(weights)
	{
		this->linkedBonesId[0] = linkedBonesId.x;
		this->linkedBonesId[1] = linkedBonesId.y;
		this->linkedBonesId[2] = linkedBonesId.z;
		this->linkedBonesId[3] = linkedBonesId.w;
		
		this->weights.Normalize();
	}

	SkinnedVertex(const Vertex& vertex, const VecInt4& linkedBonesId, const Vec4& weights) :
		SkinnedVertex(vertex.position, vertex.uv, vertex.normal, vertex.tangent, linkedBonesId, weights)
	{
	}

	// N starts from zero.
	inline bool HasNthLinkedBone(uint32 n) { return linkedBonesId[n] != 255; }

	Vec3 position;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;

	Vec3 weights; // Fourth weight is 1 - (the sum of other three weights).
	BYTE linkedBonesId[4] = { 255, 255, 255, 255 }; // 255 means no link.
};

class SkeletalMesh : public Mesh
{
public:
	SkeletalMesh(shared_ptr<Skeleton> skeleton);
	virtual ~SkeletalMesh() = default;

public:
	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	inline void Create(const vector<SkinnedVertex>& vertices, const vector<uint32>& indices, shared_ptr<Material> material = nullptr, bool addSubMeshManually = false)
	{
		Mesh::Create(vertices.data(), vertices.size(), sizeof(SkinnedVertex), indices, material, addSubMeshManually);
	}

	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	inline void Create(const vector<SkinnedVertex>& vertices, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually = false)
	{
		Mesh::Create(vertices.data(), vertices.size(), sizeof(SkinnedVertex), indices, materials, addSubMeshManually);
	}

private:
	// Hide create functions inherited from the super class.
	void Create(const vector<Vertex>& vertices, const vector<uint32>& indices, shared_ptr<Material> material = nullptr, bool addSubMeshManually = false) = delete;
	void Create(const vector<Vertex>& vertices, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually = false) = delete;
	void Create(const void* const pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, shared_ptr<Material> material = nullptr, bool addSubMeshManually = false) = delete;
	void Create(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually = false) = delete;

protected:
	virtual void OnPreRender() const override;

public:
	inline shared_ptr<Skeleton> GetSkeleton() { return _skeleton; }
	
	inline shared_ptr<AnimationInstance> GetAnimationInstance() { return _animationInstance; }
	inline shared_ptr<const AnimationInstance> GetAnimationInstance() const  { return _animationInstance; }

	inline void SetAnimationInstance(shared_ptr<AnimationInstance> animInstance) { _animationInstance = animInstance; }

private:
	shared_ptr<Skeleton> _skeleton;
	shared_ptr<AnimationInstance> _animationInstance;
};

