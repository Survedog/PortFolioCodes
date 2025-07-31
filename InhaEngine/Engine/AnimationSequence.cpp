#include "pch.h"
#include "AnimationSequence.h"
#include "Skeleton.h"

AnimationSequence::AnimationSequence() :
	Object(OBJECT_TYPE::ANIMATION_SEQUENCE),
	_targetSkeleton(nullptr)
{
}

bool AnimationSequence::GetBoneLocationValueAt(int32 boneId, float time, Vec3& outLocation)
{
	if (GetBoneAnimation(boneId) == nullptr)
	{
		outLocation = Vec3::Zero;
		return false;
	}

	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::LOCATION_X, time, outLocation.x)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::LOCATION_Y, time, outLocation.y)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::LOCATION_Z, time, outLocation.z)) return false;
	return true;
}

bool AnimationSequence::GetBoneQuatRotationValueAt(int32 boneId, float time, Quaternion& outRotation)
{
	if (GetBoneAnimation(boneId) == nullptr)
	{
		outRotation = Quaternion::Identity;
		return false;
	}

	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::QUAT_ROTATION_W, time, outRotation.w)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::QUAT_ROTATION_X, time, outRotation.x)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::QUAT_ROTATION_Y, time, outRotation.y)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::QUAT_ROTATION_Z, time, outRotation.z)) return false;
	return true;
}

bool AnimationSequence::GetBoneScaleValueAt(int32 boneId, float time, Vec3& outScale)
{
	if (GetBoneAnimation(boneId) == nullptr)
	{
		outScale = Vec3::One;
		return false;
	}

	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::SCALE_X, time, outScale.x)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::SCALE_Y, time, outScale.y)) return false;
	if (!GetBoneValueAt(boneId, ANIMATION_BONE_CHANNEL::SCALE_Z, time, outScale.z)) return false;
	return true;
}

int32 AnimationSequence::GetStartFrame() const
{
	int32 ret = INT_MAX;

	for (const shared_ptr<BoneAnimation>& boneAnim : _boneAnimations)
		if (boneAnim != nullptr)
		{
			int32 startFrame = boneAnim->GetStartFrame();
			if (startFrame != -1) ret = min(ret, startFrame);
		}
	return (ret == INT_MAX) ? -1 : ret;
}

int32 AnimationSequence::GetEndFrame() const
{
	int32 ret = -1;

	for (const shared_ptr<BoneAnimation>& boneAnim : _boneAnimations)
		if (boneAnim != nullptr)
			ret = max(ret, boneAnim->GetEndFrame());
	return ret;
}

void AnimationSequence::SetTargetSkeleton(shared_ptr<Skeleton> newSkeleton)
{
	_targetSkeleton = newSkeleton;
	_boneAnimations.clear();
	_boneAnimations.resize(newSkeleton->GetBoneCount());
}

bool AnimationFCurve::GetValueAt(float frame, float& outValue) const
{
	if (GetKeyframeCount() == 0)
	{
		// TODO: Print warning log.
		return false;
	}

	if (frame <= static_cast<float>(GetStartFrame()))
		return _keyframes[0].value;
	else if (frame >= static_cast<float>(GetEndFrame()))
		return _keyframes.back().value;
	else 
	{
		// Interpolate the value between two keyframes.
		for (int32 i = 0; i < static_cast<int32>(_keyframes.size()) - 1; ++i)
		{
			
			if (frame >= _keyframes[i].frame)
			{
				if (_keyframes[i].interpolationMode == ANIMKEY_INTERPOLATION_MODE::CONSTANT)
				{
					outValue = _keyframes[i].value;
				}
				else if (_keyframes[i].interpolationMode == ANIMKEY_INTERPOLATION_MODE::LINEAR)
				{
					float alpha = (frame - _keyframes[i].frame) / (_keyframes[i + 1].frame - _keyframes[i].frame);
					outValue = lerp(_keyframes[i].value, _keyframes[i + 1].value, alpha);
				}
				else if (_keyframes[i].interpolationMode == ANIMKEY_INTERPOLATION_MODE::CUBIC)
				{
					// TODO: Implement cubic interpolation
					float alpha = (frame - _keyframes[i].frame) / (_keyframes[i + 1].frame - _keyframes[i].frame);
					outValue = lerp(_keyframes[i].value, _keyframes[i + 1].value, alpha); // TEMP
				}
				return true;
			}
		}
	}

	assert(false); // A control flow must not go through here.
	return false;
}
