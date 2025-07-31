#pragma once

#include "Component.h"
#include "Collider.h"

class ColliderComponent : public Component
{
public:
	ColliderComponent();

public:
	void AddCollider(shared_ptr<Collider> collider);
	void RemoveCollider(uint32 idx);

	inline shared_ptr<Collider> GetFirstCollider() { return _colliders.empty() ? nullptr : _colliders[0]; }
	inline shared_ptr<const Collider> GetFirstCollider() const { return _colliders.empty() ? nullptr : _colliders[0]; }

	inline uint32 GetColliderCount() const { return static_cast<uint32>(_colliders.size()); }
	inline const vector<shared_ptr<Collider>>& GetColliders() { return _colliders; }
	inline void GetColliders(vector<shared_ptr<const Collider>>& outColliders) const { outColliders.assign(_colliders.begin(), _colliders.end()); }

protected:
	virtual void OnOwnerGameObjectSet(GameObject* newOwnerGameObject) override;

private:
	vector<shared_ptr<Collider>> _colliders;
};
