#pragma once

#include "Object.h"

class AnimationSequence;
class AnimationStateNode;

struct AnimationStateTransition
{
	AnimationStateTransition() = default;
	AnimationStateTransition(shared_ptr<AnimationStateNode> fromNode, shared_ptr<AnimationStateNode> destNode, function<bool(void)> conditionEvaluator) :
		fromNode(fromNode), destNode(destNode), conditionEvaluator(conditionEvaluator)
	{
	}

	inline bool IsConditionSatisfied()
	{
		if (conditionEvaluator == nullptr) return false;
		return conditionEvaluator();
	}

	shared_ptr<AnimationStateNode> fromNode = nullptr;
	shared_ptr<AnimationStateNode> destNode = nullptr;
	function<bool(void)> conditionEvaluator = nullptr;
};

class AnimationStateNode : public enable_shared_from_this<AnimationStateNode>
{
public:
	AnimationStateNode() = default;
	AnimationStateNode(shared_ptr<AnimationSequence> animSequence, bool doLoop = true);

public:
	inline shared_ptr<AnimationSequence> GetAnimation() { return _animSequence; }
	inline shared_ptr<const AnimationSequence> GetAnimation() const { return _animSequence; }

	inline float GetElapsedTime() const { return _elapsedTime; }
	inline float GetLength() const { return _length; }
	inline bool ShouldLoop() const { return _doLoop; }
	
	inline const vector<AnimationStateTransition>& GetTransitions() const { return _transitions; }

	void SetAnimation(shared_ptr<AnimationSequence> newAnimSequence);

	inline void AddElapsedTime(float time)
	{
		if (ShouldLoop())
		{
			_elapsedTime = fmod((GetElapsedTime() + time), GetLength());
			_elapsedTime = max(GetElapsedTime(), 0.f);
		}
		else
			_elapsedTime = clamp(GetElapsedTime() + time, 0.f, GetLength());
	}

	inline void ResetTime() { _elapsedTime = 0.f; }
	inline void SetShouldLoop(bool doLoop) { _doLoop = doLoop; }
	
	inline void AddTransition(shared_ptr<AnimationStateNode> toNode, function<bool(void)> conditionEvaluator)
	{
		_transitions.emplace_back(shared_from_this(), toNode, conditionEvaluator);
	}

	inline void RemoveTransition(uint32 idx) { _transitions.erase(_transitions.begin() + idx); }

private:
	shared_ptr<AnimationSequence> _animSequence = nullptr;

	float _elapsedTime = 0.f;
	float _length = 1.f;
	bool _doLoop = false;
	vector<AnimationStateTransition> _transitions;
};

class AnimationInstance : public Object
{
public:
	AnimationInstance();
	virtual ~AnimationInstance() = default;

public:
	inline void Play() { _isPlaying = true; }
	inline void Pause() { _isPlaying = false; }
	inline void ResetCurrentAnimation()
	{
		shared_ptr<AnimationStateNode> node = GetCurrentAnimationStateNode();
		if (node != nullptr) node->ResetTime();
		Pause();
	}
	
	// Returns if the animation is being played or not.
	// If the game timer is stopped, it will return false even if _isPlaying member variable is set to true.
	bool IsPlaying() const;

public:
	virtual void OnAnimationInit() {};
	virtual void OnAnimationTick();

public:
	// Calculates bone matrices(bind pose local space to animation pose local space) and inverse transpose version of them.
	// Will skip actual calculation if input animation time is almost same as the last calculation's animation time.
	void CalcBoneMatrices(float animationTime);

	void PushData() const;

public:
	inline shared_ptr<AnimationSequence> GetCurrentAnimationSequence() { return const_pointer_cast<AnimationSequence>(CONST_THIS->GetCurrentAnimationSequence()); }
	inline shared_ptr<const AnimationSequence> GetCurrentAnimationSequence() const
	{
		shared_ptr<const AnimationStateNode> stateNode = GetCurrentAnimationStateNode();
		return stateNode != nullptr ? stateNode->GetAnimation() : nullptr;
	}
	
	inline shared_ptr<AnimationStateNode> GetCurrentAnimationStateNode() { return GetAnimationStateNode(_currentStateNodeIdx); }
	inline shared_ptr<const AnimationStateNode> GetCurrentAnimationStateNode() const { return GetAnimationStateNode(_currentStateNodeIdx); }

	inline shared_ptr<AnimationStateNode> GetAnimationStateNode(uint32 nodeIdx)
	{
		if (nodeIdx >= static_cast<uint32>(_animStateNodes.size())) return nullptr;
		return _animStateNodes[nodeIdx];
	}

	inline shared_ptr<const AnimationStateNode> GetAnimationStateNode(uint32 nodeIdx) const
	{
		if (nodeIdx >= static_cast<uint32>(_animStateNodes.size())) return nullptr;
		return _animStateNodes[nodeIdx];
	}

	inline void AddAnimationStateNode(shared_ptr<AnimationSequence> animSequence, bool doLoop = true)
	{
		_animStateNodes.push_back(make_shared<AnimationStateNode>(animSequence, doLoop));
	}

	inline void AddAnimationStateTransition(uint32 fromNodeIdx, uint32 toNodeIdx, function<bool(void)> conditionEvaluator)
	{
		if (fromNodeIdx >= static_cast<uint32>(_animStateNodes.size()) || toNodeIdx >= static_cast<uint32>(_animStateNodes.size())
			|| _animStateNodes[fromNodeIdx] == nullptr || _animStateNodes[toNodeIdx] == nullptr)
		{
			// TODO: Print error log.
			return;
		}
		_animStateNodes[fromNodeIdx]->AddTransition(_animStateNodes[toNodeIdx], conditionEvaluator);
	}

private:
	inline bool GetBoneMatrix(uint32 idx, Matrix& outMatrix) const
	{
		if (idx >= static_cast<uint32>(_boneMatrices.size()))
		{
			// TODO: Print error log.
			return false;
		}
		outMatrix = _boneMatrices[idx];
		return true;
	}

private:
	bool _isPlaying = false;

	// Transforms a vertex from bind space(object local space in bind pose) to object local space.
	// It is a multiplication result of bind-to-bone(offset) matrix and bone-to-local(to-root) matrix.
	vector<Matrix> _boneMatrices;

	/* Animation state machine */ 
	uint32 _currentStateNodeIdx = 0;	
	vector<shared_ptr<AnimationStateNode>> _animStateNodes;
};

