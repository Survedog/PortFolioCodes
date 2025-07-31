#pragma once

// NOTE: When you add a new object type, you must also update the GetObjectType() function in Object class.
enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT, // PREFAB
	COMPONENT,
	MATERIAL,
	STATIC_MESH,
	SKELETAL_MESH,
	SKELETON,
	ANIMATION_SEQUENCE,
	ANIMATION_INSTANCE,
	SHADER,
	TEXTURE,
	SCENE,
	SCENE_RENDERER,
	INPUT_MAPPING_CONTEXT,
	HEIGHT_MAP,
	SKYBOX,
	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
	Object(OBJECT_TYPE objectType, wstring name = L"NoName");
	virtual ~Object();

public:
	template<typename T>
	OBJECT_TYPE static GetObjectType()
	{
		if (is_same_v<T, class GameObject>) return OBJECT_TYPE::GAMEOBJECT;
		if (is_same_v<T, class Component>) return OBJECT_TYPE::COMPONENT;
		if (is_same_v<T, class Material>) return OBJECT_TYPE::MATERIAL;
		if (is_same_v<T, class Mesh>) return OBJECT_TYPE::STATIC_MESH;
		if (is_same_v<T, class SkeletalMesh>) return OBJECT_TYPE::SKELETAL_MESH;
		if (is_same_v<T, class Skeleton>) return OBJECT_TYPE::SKELETON;
		if (is_same_v<T, class AnimationSequence>) return OBJECT_TYPE::ANIMATION_SEQUENCE;
		if (is_same_v<T, class Shader>) return OBJECT_TYPE::SHADER;
		if (is_same_v<T, class Texture>) return OBJECT_TYPE::TEXTURE;
		if (is_same_v<T, class Scene>) return OBJECT_TYPE::SCENE;
		if (is_same_v<T, class SceneRenderer>) return OBJECT_TYPE::SCENE_RENDERER;
		if (is_same_v<T, class InputMappingContext>) return OBJECT_TYPE::INPUT_MAPPING_CONTEXT;
		if (is_same_v<T, class HeightMap>) return OBJECT_TYPE::HEIGHT_MAP;
		if (is_same_v<T, class Skybox>) return OBJECT_TYPE::SKYBOX;
		return OBJECT_TYPE::NONE;
	}

	inline OBJECT_TYPE GetObjectType() const { return _objectType; }
	inline uint32 GetID() const { return _id; }

	inline const wstring& GetName() const { return _name; }
	inline void SetName(const wstring& name) { _name = name; }

	virtual void Load(const wstring& path) {}
	virtual void Save(const wstring& path) {}

protected:
	// WARNING: Use this only in the constructor.
	inline void SetObjectType(OBJECT_TYPE newType) { _objectType = newType; }

private:
	OBJECT_TYPE _objectType;
	uint32 _id;
	wstring _name;
};

