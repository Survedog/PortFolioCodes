#pragma once

#include "Object.h"

class Shader;
class Texture;

// Warning: When adding/removing enum element, you must also adjust the byte alignment of the struct MaterialConstants. (As TEXTURE_TYPE_COUNT value changes)
enum class TEXTURE_TYPE : uint8
{
	DIFFUSE,
	SPECULAR,
	ROUGHNESS,
	METALLIC,
	EMISSIVE,
	OPACITY,
	NORMAL,
	WORLD_POSITION_OFFSET,
	WORLD_DISPLACEMENT,
	AMBIENT_OCCLUSION,
	TESSELLATION_MULTIPLIER,
	REFRACTION,
	MISC,
	END
};

enum
{
	MATERIAL_ARG_COUNT = 4,
	TEXTURE_TYPE_COUNT = static_cast<uint8>(TEXTURE_TYPE::END),
	MAX_TEXTURE_CUBES = 1,
};

struct MaterialColor
{
	Vec4 diffuseAlbedo;
	Vec3 specular;
	float roughness;
};

//TODO: Implement using multiple materials.
struct MaterialConstants
{
public:
	inline const Vec4& GetDiffuseColor() const { return diffuseColor; }
	inline const Vec3& GetSpecular() const { return specular; }
	inline float GetRoughness() const { return roughness; }

	inline float GetUScale() const { return uScale; }
	inline float GetVScale() const { return uScale; }

	inline void SetDiffuseColor(const Vec4& newDiffuseColor) { diffuseColor = newDiffuseColor; }
	inline void SetSpecular(const Vec3& newSpecular) { specular = newSpecular; }
	inline void SetRoughness(float newRoughness) { roughness = newRoughness; }

	inline void SetUScale(float newUScale) { uScale = newUScale; }
	inline void SetVScale(float newVScale) { vScale = newVScale; }

	inline void SetTexOn(uint8 index, int32 value) { assert(index < TEXTURE_TYPE_COUNT); texOnParams[index] = value; }
	inline void SetTexCubeOn(uint8 index, int32 value) { assert(index < MAX_TEXTURE_CUBES); texCubeOnParams[index] = value; }

private:
	Vec3 specular = Vec3(1.f); // If specular texture is set, it's overrided by the texture's color.
	float roughness = 0.5f; // If roughness texture is set, it's overrided by the texture's color.
	Vec4 diffuseColor = Vec4(0.f, 0.f, 0.f, 1.f); // If diffuse texture is set, it's overrided by the texture's color.
	
	float uScale = 1.f;
	float vScale = 1.f;
	std::array<int32, TEXTURE_TYPE_COUNT> texOnParams = {};
	std::array<int32, MAX_TEXTURE_CUBES> texCubeOnParams = {};
	Vec3 __padding1;
};

class Material : public Object
{
public:
	Material();
	virtual ~Material() = default;

public:
	void PushData();
	void ApplyShader(ComPtr<ID3D12GraphicsCommandList> commandList);

public:
	inline shared_ptr<Shader> GetShader() { return _shader; }
	inline shared_ptr<const Shader> GetShader() const { return _shader; }

	inline void SetShader(shared_ptr<Shader> newShader) { _shader = newShader; }

	inline shared_ptr<Texture> GetTexture(int index) { assert(index < TEXTURE_TYPE_COUNT && "The texture index is out of bounds."); return _textures[index]; }
	inline shared_ptr<const Texture> GetTexture(int index) const { assert(index < TEXTURE_TYPE_COUNT && "The texture index is out of bounds."); return _textures[index]; }
	inline const vector<shared_ptr<Texture>>& GetTextures() const { return _textures; }

	inline shared_ptr<Texture> GetDiffuseTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::DIFFUSE)); }
	inline shared_ptr<Texture> GetSpecularTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::SPECULAR)); }
	inline shared_ptr<Texture> GetRoughnessTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::ROUGHNESS)); }
	inline shared_ptr<Texture> GetMetallicTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::METALLIC)); }
	inline shared_ptr<Texture> GetEmissiveTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::EMISSIVE)); }
	inline shared_ptr<Texture> GetOpacityTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::OPACITY)); }
	inline shared_ptr<Texture> GetAmbientOcclusionTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::AMBIENT_OCCLUSION)); }
	inline shared_ptr<Texture> GetNormalTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::NORMAL)); }
	inline shared_ptr<Texture> GetMiscTexture() { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::MISC)); }

	inline shared_ptr<const Texture> GetDiffuseTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::DIFFUSE)); }
	inline shared_ptr<const Texture> GetSpecularTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::SPECULAR)); }
	inline shared_ptr<const Texture> GetRoughnessTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::ROUGHNESS)); }
	inline shared_ptr<const Texture> GetMetallicTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::METALLIC)); }
	inline shared_ptr<const Texture> GetEmissiveTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::EMISSIVE)); }
	inline shared_ptr<const Texture> GetOpacityTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::OPACITY)); }
	inline shared_ptr<const Texture> GetAmbientOcclusionTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::AMBIENT_OCCLUSION)); }
	inline shared_ptr<const Texture> GetNormalTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::NORMAL)); }
	inline shared_ptr<const Texture> GetMiscTexture() const { return GetTexture(static_cast<uint8>(TEXTURE_TYPE::MISC)); }

	inline const vector<shared_ptr<Texture>>& GetTextureCubes() const { return _textureCubes; }
	inline shared_ptr<Texture> GetTextureCube(int index) { assert(index < MAX_TEXTURE_CUBES && "The texture cube index is out of bounds."); return _textureCubes[index]; }
	inline shared_ptr<const Texture> GetTextureCube(int index) const { assert(index < MAX_TEXTURE_CUBES && "The texture cube index is out of bounds."); return _textureCubes[index]; }

	// If diffuse texture is set, it's overrided by the texture's color.
	inline const Vec4& GetDiffuseColor() { return _params.GetDiffuseColor(); }
	// If roughness texture is set, it's overrided by the texture's color.
	inline float GetRoughness() { return _params.GetRoughness(); }

	inline const Vec3& GetSpecular() { return _params.GetSpecular(); }
	inline float GetUScale() const { return _params.GetUScale(); }
	inline float GetVScale() const { return _params.GetVScale(); }

	// If diffuse texture is set, it's overrided by the texture's color.
	inline void SetDiffuseColor(const Vec3& newDiffuseColor) { _params.SetDiffuseColor(Vec4(newDiffuseColor.x, newDiffuseColor.y, newDiffuseColor.z, 1.f)); }
	inline void SetDiffuseColor(const Vec4& newDiffuseColor) { _params.SetDiffuseColor(newDiffuseColor); }
	// If specular texture is set, it's overrided by the texture's color.
	inline void SetSpecular(const Vec3& newSpecular) { _params.SetSpecular(newSpecular); }
	// If roughness texture is set, it's overrided by the texture's color.
	inline void SetRoughness(float newRoughness) { _params.SetRoughness(newRoughness); }

	inline void SetUScale(float newUScale) { _params.SetUScale(newUScale); }
	inline void SetVScale(float newVScale) { _params.SetVScale(newVScale); }

	inline void SetDiffuseTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::DIFFUSE, texture); }
	inline void SetSpecularTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::SPECULAR, texture); }
	inline void SetRoughnessTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::ROUGHNESS, texture); }
	inline void SetMetallicTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::METALLIC, texture); }
	inline void SetEmissiveTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::EMISSIVE, texture); }
	inline void SetOpacityTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::OPACITY, texture); }
	inline void SetAmbientOcclusionTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::AMBIENT_OCCLUSION, texture); }
	inline void SetNormalTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::NORMAL, texture); }
	inline void SetMiscTexture(shared_ptr<Texture> texture) { SetTexture(TEXTURE_TYPE::MISC, texture); }

	inline void SetTexture(TEXTURE_TYPE type, shared_ptr<Texture> texture)
	{
		uint8 typeIdx = static_cast<uint8>(type);
		_textures[typeIdx] = texture;
		_params.SetTexOn(typeIdx, (texture == nullptr ? 0 : 1));
	}

	inline void SetTextureCube(shared_ptr<Texture> textureCube, uint32 index = 0)
	{
		_textureCubes[index] = textureCube;
		_params.SetTexCubeOn(index, (textureCube == nullptr ? 0 : 1));
	}

private:
	shared_ptr<Shader> _shader;
	MaterialConstants _params;
	vector<shared_ptr<Texture>> _textures;
	vector<shared_ptr<Texture>> _textureCubes;
};

