#include "pch.h"
#include "SkeletalMesh.h"
#include "AnimationSequence.h"
#include "AnimationInstance.h"

SkeletalMesh::SkeletalMesh(shared_ptr<Skeleton> skeleton) : Mesh(), _skeleton(skeleton), _animationInstance(nullptr)
{
	SetObjectType(OBJECT_TYPE::SKELETAL_MESH);
}

void SkeletalMesh::OnPreRender() const
{
	shared_ptr<const AnimationInstance> animInstance = GetAnimationInstance();
	animInstance->PushData();
}