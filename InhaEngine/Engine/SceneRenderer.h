#pragma once

#include "Object.h"

class Scene;

class SceneRenderer : public Object
{
public:
	SceneRenderer();
	virtual ~SceneRenderer() = default;

public:
	virtual void RenderScene(shared_ptr<Scene> scene) const;

protected:
	void PushGlobalTextures() const;

	virtual void RenderForward(shared_ptr<Scene> scene) const;
	virtual void RenderShadowMap(shared_ptr<Scene> scene) const;
};

