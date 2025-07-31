#pragma once

#include "Object.h"

struct Bone
{
	Bone() = default;
	
	Bone(const wstring& name, const Transform& transform, float length, shared_ptr<Bone> parent = nullptr)
		: id(-1), name(name), transform(transform), length(length), parent(parent)
	{
	}

	Bone(int32 id, const wstring& name, const Transform& transform, float length, shared_ptr<Bone> parent = nullptr)
		: id(id), name(name), transform(transform), length(length), parent(parent)
	{
	}

	int32 id;
	wstring name;
	Transform transform; // Local transform of the bone in bind pose.
	float length;
	
	weak_ptr<Bone> parent; // Parent bone.
	vector<shared_ptr<Bone>> children; // Child bones.
};

class Skeleton : public Object
{
public:
	Skeleton();

public:	
	inline bool GetOffsetMatrix(uint32 idx, Matrix& outMatrix)
	{
		if (idx >= _offsetMatrices.size())
		{
			//TODO: Print error log.
			return false;
		}
		outMatrix = _offsetMatrices[idx];
		return true;
	}

	inline const vector<Matrix>& GetOffsetMatrices() { return _offsetMatrices; }

public:
	inline shared_ptr<const Bone> GetRootBone() const
	{
		if (_bones.empty())
		{
			// TODO: Print error log.
			return nullptr;
		}
		return _bones[0]; // Root bone is always at index 0.
	}
	inline shared_ptr<Bone> GetRootBone() { return const_pointer_cast<Bone>(CONST_THIS->GetRootBone()); }	


	inline shared_ptr<const Bone> GetBoneById(uint32 id) const
	{
		if (id >= static_cast<uint32>(_bones.size()))
		{
			// TODO: Print error log.
			return nullptr;
		}
		return _bones[id];
	}
	inline shared_ptr<Bone> GetBoneById(uint32 id) { return const_pointer_cast<Bone>(CONST_THIS->GetBoneById(id)); }

	// The index for a bone in this vector is that bone's id.
	inline const vector<shared_ptr<Bone>>& GetBones() { return _bones; }

	inline int32 FindBoneIdByName(const wstring& boneName)
	{
		auto findIter = _boneNameIdMap.find(boneName);
		return (findIter != _boneNameIdMap.end()) ? findIter->second : -1;
	}

	inline shared_ptr<Bone> FindBoneByName(const wstring& boneName)
	{
		int32 boneId = FindBoneIdByName(boneName);
		return (boneId == -1) ? nullptr : _bones[boneId];
	}

	inline uint32 GetBoneCount() const { return static_cast<uint32>(_bones.size()); }

	void SetRootBone(shared_ptr<Bone> rootBone);

private:
	void GetBonesInHierarchy(shared_ptr<Bone> rootBone, vector<shared_ptr<Bone>>& outBones);
	void CalcOffsetMatrices();

private:
	map<wstring, int32> _boneNameIdMap;
	vector<shared_ptr<Bone>> _bones;

	// Transforms a vertex from bind space(object local space in bind pose) to bone local space.
	vector<Matrix> _offsetMatrices;
};

