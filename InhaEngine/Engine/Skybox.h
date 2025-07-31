#pragma once

#include "Object.h"

class Mesh;
class Material;

class Skybox : public Object
{

public:
	Skybox();
	virtual ~Skybox() = default;

	void RenderSkybox() const;

public:
	shared_ptr<Mesh> GetMesh() const { return _mesh; }
	shared_ptr<Material> GetMaterial() const { return _material; }

	void SetMesh(shared_ptr<Mesh> newMesh) { _mesh = newMesh; }
	void SetMaterial(shared_ptr<Material> newMaterial) { _material = newMaterial; }

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
};

