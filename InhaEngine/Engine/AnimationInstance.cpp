#include "pch.h"
#include "Skeleton.h"
#include "AnimationInstance.h"
#include "AnimationSequence.h"
#include "Engine.h"
#include "Timer.h"
#include "TableDescriptorHeapManager.h"

AnimationStateNode::AnimationStateNode(shared_ptr<AnimationSequence> animSequence, bool doLoop) :
	_animSequence(animSequence), _doLoop(doLoop)
{
	if (animSequence != nullptr) _length = animSequence->GetLength();
}

void AnimationStateNode::SetAnimation(shared_ptr<AnimationSequence> newAnimSequence)
{
	_animSequence = newAnimSequence;
	if (newAnimSequence != nullptr)
		_length = newAnimSequence->GetLength();
}

AnimationInstance::AnimationInstance() :
	Object(OBJECT_TYPE::ANIMATION_INSTANCE)
{
}

bool AnimationInstance::IsPlaying() const
{
	return _isPlaying && !ENGINE->GetGameTimer()->IsTimerStopped();
}

void AnimationInstance::OnAnimationTick()
{
	if (!IsPlaying()) return;
	
	shared_ptr<AnimationStateNode> animStateNode = GetCurrentAnimationStateNode();
	if (animStateNode != nullptr && animStateNode->GetAnimation() != nullptr)
	{
		float deltaTime = ENGINE->GetGameTimer()->GetDeltaTime();
		animStateNode->AddElapsedTime(deltaTime);
		if (deltaTime > 0.00001f) CalcBoneMatrices(animStateNode->GetElapsedTime());
	}
}

void AnimationInstance::CalcBoneMatrices(float animationTime)
{
	shared_ptr<AnimationSequence> animSequence = GetCurrentAnimationSequence();
	if (animSequence == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	shared_ptr<Skeleton> skeleton = animSequence->GetTargetSkeleton();
	if (skeleton == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	//TODO: Use caching if the pose has not changed.
	const vector<shared_ptr<Bone>>& bones = skeleton->GetBones();
	_boneMatrices.clear();
	_boneMatrices.resize(bones.size(), Matrix::Identity);

	// Calculate bone-to-local(to-root) matrices first.	
	for (size_t boneId = 0; boneId < bones.size(); ++boneId)
	{
		const shared_ptr<Bone>& bone = bones[boneId];
		assert(bone != nullptr && bone->id == boneId);

		Vec3 animatedPosition = Vec3::Zero;
		Quaternion animatedRotation = Quaternion::Identity;
		Vec3 animatedScale = Vec3::One;

		// Get bone's local transform in animation.
		animSequence->GetBoneLocationValueAt(boneId, animationTime, animatedPosition);
		animSequence->GetBoneQuatRotationValueAt(boneId, animationTime, animatedRotation);
		animSequence->GetBoneScaleValueAt(boneId, animationTime, animatedScale);

		// Calculate animation transform matrix.
		_boneMatrices[boneId] = DirectX::XMMatrixTransformation(
				g_XMZero,
				Quaternion::Identity,
				GetDxCoordsFromGameCoords(animatedScale),
				g_XMZero,
				animatedRotation,
				GetDxCoordsFromGameCoords(animatedPosition));

		// Calculate bone local transform matrix and multiply it to animation matrix.
		_boneMatrices[boneId] *= DirectX::XMMatrixTransformation(
			g_XMZero,
			Quaternion::Identity,
			GetDxCoordsFromGameCoords(bone->transform.scale),
			g_XMZero,
			bone->transform.rotation,
			GetDxCoordsFromGameCoords(bone->transform.position));

		// NOTE: Bones in _bones are placed in depth-first search order,
		// so the calculation for every bone is done prior to its children.
		if (shared_ptr<Bone> parentBone = bone->parent.lock())
			_boneMatrices[boneId] = _boneMatrices[boneId] * _boneMatrices[parentBone->id];
	}

	// Calculate bone matrices.	
	const vector<Matrix>& offsetMatrices = skeleton->GetOffsetMatrices();
	assert(offsetMatrices.size() == bones.size());

	for (size_t boneId = 0; boneId < bones.size(); ++boneId)
	{
		_boneMatrices[boneId] = offsetMatrices[boneId] * _boneMatrices[boneId];
	}
}

void AnimationInstance::PushData() const
{
	shared_ptr<const AnimationSequence> animSequence = GetCurrentAnimationSequence();
	if (animSequence == nullptr || animSequence->GetTargetSkeleton() == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PerAnimationConstants animationConstants;
	for (uint32 i = 0; i < static_cast<uint32>(_boneMatrices.size()); ++i)
	{
		GetBoneMatrix(i, animationConstants.boneMatrices[i]);
		animationConstants.boneInvTransposeMatrices[i] = animationConstants.boneMatrices[i].Invert().Transpose(); // Used for calculating normals. (Not needed if there's no non-uniform animatedScale transformation)
	}

	ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::ANIMATION);
	ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::PER_ANIMATION)->PushGraphicsConstants(&animationConstants, sizeof(animationConstants));
	ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::ANIMATION), ANIMATION_TABLE_DESC_COUNT);
}
