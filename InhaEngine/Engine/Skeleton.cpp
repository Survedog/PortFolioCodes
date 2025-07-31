#include "pch.h"
#include "Skeleton.h"

Skeleton::Skeleton() :
	Object(OBJECT_TYPE::SKELETON)
{
}

void Skeleton::SetRootBone(shared_ptr<Bone> rootBone)
{
	_bones.clear();
	_boneNameIdMap.clear();

	if (rootBone != nullptr)
	{
		vector<shared_ptr<Bone>> bones;
		GetBonesInHierarchy(rootBone, bones);
		
		assert(static_cast<uint32>(bones.size()) <= MAX_SKELETON_BONES);
		_bones.reserve(bones.size());

		for (shared_ptr<Bone>& bone : bones)
		{
			assert(bone != nullptr);
			
			_bones.push_back(bone);
			int32 boneId = static_cast<int32>(_bones.size()) - 1;
			bone->id = boneId;
			_boneNameIdMap[bone->name] = boneId;
		}		

		CalcOffsetMatrices();
	}
}

void Skeleton::GetBonesInHierarchy(shared_ptr<Bone> rootBone, vector<shared_ptr<Bone>>& outBones)
{
	if (rootBone == nullptr) return;

	outBones.push_back(rootBone);
	for (shared_ptr<Bone>& childBone : rootBone->children)
		GetBonesInHierarchy(childBone, outBones);
}

void Skeleton::CalcOffsetMatrices()
{
	_offsetMatrices.clear();
	_offsetMatrices.resize(_bones.size(), Matrix::Identity);

	// Calculate bone-to-local matrices in bind pose.
	for (size_t boneId = 0; boneId < _bones.size(); ++boneId)
	{
		const shared_ptr<Bone>& bone = GetBoneById(boneId);
		assert(bone != nullptr && bone->id == boneId);

		_offsetMatrices[boneId] = DirectX::XMMatrixTransformation(
			g_XMZero,
			Quaternion::Identity,
			GetDxCoordsFromGameCoords(bone->transform.scale),
			g_XMZero,
			bone->transform.rotation,
			GetDxCoordsFromGameCoords(bone->transform.position));

		// NOTE: Bones in _bones are placed in depth-first search order,
		// so the calculation for every bone is done prior to its children.
		if (shared_ptr<Bone> parentBone = bone->parent.lock())
			_offsetMatrices[boneId] = _offsetMatrices[boneId] * _offsetMatrices[parentBone->id];
	}

	// Invert the matrices to get bind-to-bone matrices.
	for (size_t boneId = 0; boneId < _bones.size(); ++boneId)
	{
		_offsetMatrices[boneId] = _offsetMatrices[boneId].Invert();
	}
}
