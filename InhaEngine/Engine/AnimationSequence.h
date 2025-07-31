#pragma once

#include "Object.h"

class Skeleton;

enum class ANIMKEY_INTERPOLATION_MODE : uint8
{
	CONSTANT,
	LINEAR,
	CUBIC
};

enum class ANIMATION_BONE_CHANNEL : uint8
{
	LOCATION_X,
	LOCATION_Y,
	LOCATION_Z,
	QUAT_ROTATION_X,
	QUAT_ROTATION_Y,
	QUAT_ROTATION_Z,
	QUAT_ROTATION_W,
	SCALE_X,
	SCALE_Y,
	SCALE_Z,
	END // Warning: Not an actual enum item!!
}; 

enum
{
	ANIMATION_BONE_CHANNEL_COUNT = static_cast<uint8>(ANIMATION_BONE_CHANNEL::END)
};

struct AnimationKeyframe
{
	AnimationKeyframe() = default;

	AnimationKeyframe(int32 frame, float value) :
		frame(frame), value(value)
	{
	}

	AnimationKeyframe(int32 frame, float value, ANIMKEY_INTERPOLATION_MODE interpolationMode) :
		frame(frame), value(value), interpolationMode(interpolationMode)
	{
	}

	int32 frame = -1;
	float value = 0.f;
	ANIMKEY_INTERPOLATION_MODE interpolationMode = ANIMKEY_INTERPOLATION_MODE::LINEAR;
};

class AnimationFCurve
{
public:
	AnimationFCurve() = default;

public:
	bool GetValueAt(float frame, float& outValue) const;

	// Returns -1 if it has no keyframes.
	inline int32 GetStartFrame() const { return _keyframes.empty() ? -1 : _keyframes[0].frame; }

	// Returns -1 if it has no keyframes.
	inline int32 GetEndFrame() const { return _keyframes.empty() ? -1 : _keyframes.back().frame; }

	// Returns nullopt if there's no keyframe.
	inline optional<AnimationKeyframe> GetKeyframe(int32 frame)
	{
		for (int32 i = 0; i < GetKeyframeCount(); ++i)
			if (frame == _keyframes[i].frame)
				return _keyframes[i];
			else if (frame > _keyframes[i].frame)
				return nullopt;
	}

	// Returns nullopt if there's no keyframe.
	inline optional<AnimationKeyframe> GetKeyframeByIndex(int32 index)
	{
		if (index < 0 || index >= GetKeyframeCount())
		{
			//TODO: Print error log.
			return nullopt;
		}
		return _keyframes[index];
	}

	vector<AnimationKeyframe>& GetKeyframes() { return _keyframes; }
	const vector<AnimationKeyframe>& GetKeyframes() const { return _keyframes; }

	inline void InsertKeyframe(int32 frame, float value) { InsertKeyframe(AnimationKeyframe(frame, value)); }
	inline void InsertKeyframe(const AnimationKeyframe& newKeyframe) { InsertKeyframe(move(newKeyframe)); }
	void InsertKeyframe(AnimationKeyframe&& newKeyframe)
	{
		auto iter = _keyframes.begin();
		while (iter != _keyframes.end())
		{
			if (newKeyframe.frame <= iter->frame)
			{
				_keyframes.insert(iter, newKeyframe);
				return;
			}
			iter++;
		}

		_keyframes.push_back(newKeyframe);
	}

	// Delete the first keyframe that is on a specific frame.
	inline void DeleteKeyframe(int32 frame)
	{
		for (int32 i = 0; i < GetKeyframeCount(); ++i)
			if (frame == _keyframes[i].frame)
			{
				DeleteKeyframeByIndex(i);
				break;
			}
	}

	inline void DeleteKeyframeByIndex(int32 index)
	{
		if (index < 0 || index >= GetKeyframeCount())
		{
			//TODO: Print error log.
			return;
		}
		_keyframes.erase(_keyframes.begin() + index);
	}

	inline uint32 GetKeyframeCount() const { return static_cast<uint32>(_keyframes.size()); }

private:
	vector<AnimationKeyframe> _keyframes;
};

// Represents an animation clip.
class BoneAnimation
{
public:
	BoneAnimation(int32 targetBoneId) : _targetBoneId(targetBoneId)
	{
		fill_n(_fCurves, ANIMATION_BONE_CHANNEL_COUNT, nullptr);
	}

public:
	inline int32 GetTargetBoneId() { return _targetBoneId; }

	// Returns -1 if it has no keyframes.
	inline int32 GetStartFrame() const
	{
		int32 ret = INT_MAX;

		for (const shared_ptr<AnimationFCurve>& fcurve : _fCurves)
			if (fcurve != nullptr)
			{
				int32 startFrame = fcurve->GetStartFrame();
				if (startFrame != -1) ret = min(ret, startFrame);
			}
		return (ret == INT_MAX) ? -1 : ret;
	}

	// Returns -1 if it has no keyframes.
	inline int32 GetEndFrame() const
	{
		int32 ret = -1;

		for (const shared_ptr<AnimationFCurve>& fcurve : _fCurves)
			if (fcurve != nullptr)
				ret = max(ret, fcurve->GetEndFrame());
		return ret;
	}	

public:
	inline bool GetValueAt(ANIMATION_BONE_CHANNEL channel, float frame, float& outValue) 
	{
		if (GetChannelFCurve(channel) == nullptr) return false;
		return GetChannelFCurve(channel)->GetValueAt(frame, outValue);
	}

	inline shared_ptr<AnimationFCurve> GetChannelFCurve(ANIMATION_BONE_CHANNEL channel) { return _fCurves[static_cast<uint8>(channel)]; }
	inline void SetChannelFCurve(shared_ptr<AnimationFCurve> fCurve, ANIMATION_BONE_CHANNEL channel) { _fCurves[static_cast<uint8>(channel)] = fCurve; }

	inline bool IsEmpty() {
		for (uint8 i = 0; i < ANIMATION_BONE_CHANNEL_COUNT; ++i)
			if (GetChannelFCurve(static_cast<ANIMATION_BONE_CHANNEL>(i)) != nullptr)
				return false;
		return true;
	}

private:
	int32 _targetBoneId = -1;	
	shared_ptr<AnimationFCurve> _fCurves[ANIMATION_BONE_CHANNEL_COUNT];
	
};

// Represents a single animation clip.
class AnimationSequence : public Object
{
public:
	AnimationSequence();

public:
	inline bool GetBoneValueAt(int32 boneId, ANIMATION_BONE_CHANNEL channel, float animationTime, float& outValue)
	{
		if (GetBoneAnimation(boneId) == nullptr) return false;
		float frame = animationTime * GetSampleRate() * GetPlayRateScale();
		return GetBoneAnimation(boneId)->GetValueAt(channel, frame, outValue);
	}

	bool GetBoneLocationValueAt(int32 boneId, float time, Vec3& outLocation);
	bool GetBoneEulerRotationValueAt(int32 boneId, float time, Vec3& outRotation);
	bool GetBoneQuatRotationValueAt(int32 boneId, float time, Quaternion& outRotation);
	bool GetBoneScaleValueAt(int32 boneId, float time, Vec3& outScale);

	int32 GetStartFrame() const;
	int32 GetEndFrame() const;

	inline shared_ptr<BoneAnimation> GetBoneAnimation(int32 boneId) { return const_pointer_cast<BoneAnimation>(CONST_THIS->GetBoneAnimation(boneId)); }
	inline shared_ptr<const BoneAnimation> GetBoneAnimation(int32 boneId) const
	{
		if (boneId < 0 || boneId >= static_cast<int32>(_boneAnimations.size()))
		{
			// TODO: Print error log.
			return nullptr;
		}
		return _boneAnimations[boneId];
	}

	inline void SetBoneAnimation(shared_ptr<BoneAnimation> newBoneAnimation)
	{
		if (newBoneAnimation == nullptr || newBoneAnimation->GetTargetBoneId() < 0
			|| newBoneAnimation->GetTargetBoneId() >= static_cast<int32>(_boneAnimations.size()))
		{
			// TODO: Print error log.
			return;
		}
		_boneAnimations[newBoneAnimation->GetTargetBoneId()] = newBoneAnimation;
	}

	inline void RemoveBoneAnimation(int32 boneId)
	{
		if (boneId < 0 || boneId >= static_cast<int32>(_boneAnimations.size()))
		{
			// TODO: Print error log.
			return;
		}
		_boneAnimations[boneId] = nullptr;
	}
	
public:
	inline float GetLength() const
	{
		int32 endFrame = GetEndFrame();
		if (endFrame < 0) return 0.f;
		return GetEndFrame() / GetSampleRate() / GetPlayRateScale(); // TODO: Find a way to reduce overhead.
	}

	inline float GetSampleRate() const { return _sampleRate; }
	inline void SetSampleRate(float newSampleRate) { _sampleRate = newSampleRate; }

	inline float GetPlayRateScale() const { return _playRateScale; }
	inline void SetPlayRateScale(float newScale) { _playRateScale = newScale; }

	inline bool ShouldPlayLoop() const { return _playLoop; }
	inline void SetPlayLoop(bool doLoop) { _playLoop = doLoop; }	

	inline shared_ptr<Skeleton> GetTargetSkeleton() { return _targetSkeleton; }
	inline shared_ptr<const Skeleton> GetTargetSkeleton() const { return _targetSkeleton; }
	void SetTargetSkeleton(shared_ptr<Skeleton> newSkeleton);

private:
	float _sampleRate = 30.f;
	float _playRateScale = 1.f;
	bool _playLoop = false;
	
	shared_ptr<Skeleton> _targetSkeleton;
	vector<shared_ptr<BoneAnimation>> _boneAnimations;
};

