#pragma once

/*** Typedefs ***/
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using VecInt2 = DirectX::XMINT2;
using VecInt3 = DirectX::XMINT3;
using VecInt4 = DirectX::XMINT4;
using VecUInt2 = DirectX::XMUINT2;
using VecUInt3 = DirectX::XMUINT3;
using VecUInt4 = DirectX::XMUINT4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

/******* Constants *******/
// Registers
enum class CBV_REGISTER : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class SRV_REGISTER : uint8
{
	t0,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,
	t11,
	t12,
	t13,
	t14,
	t15,
	t16,
	t17,
	t18,
	t19,
	t20,

	END
};

enum class UAV_REGISTER : uint8
{
	u0,
	u1,
	u2,
	u3,
	u4,

	END,
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	SHADOW_MAP_TEXTURE_COUNT = 3,
	CBV_REGISTER_COUNT = static_cast<uint8>(CBV_REGISTER::END),
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END),
	UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END),
	CBV_SRV_REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,
	TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT
};

enum class ROOT_PARAM : uint8
{	
	MATERIAL,
	PASS,
	GLOBAL,
	GLOBAL_TEXTURE,
	TRANSFORM, // TEMP
	ANIMATION,
	NONE,
};

enum class DESC_TABLE_TYPE : uint8
{
	NONE,
	MATERIAL,
	PASS,
	GLOBAL_TEXTURE,
	TRANSFORM, // TEMP
	ANIMATION,
};

// Must set MATERIAL_TABLE_DESC_COUNT same as the size of this enum.
// And MATERIAL_TABLE_SRV_COUNT must be set to the number of texture registers.
enum class MATERIAL_TABLE_REGISTER : uint8
{
	b0,
	t0,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,
	t11,
	t12,
	t13,
	END // Warning: Not an actual enum element. (Use only for checking the size of this enum.)
};

// Must set PASS_TABLE_DESC_COUNT same as the size of this enum.
enum class PASS_TABLE_REGISTER : uint8
{
	b1,
	END // Warning: Not an actual enum element. (Use only for checking the size of this enum.)
};

// Must set GLOBAL_TEXTURE_TABLE_DESC_COUNT same as the size of this enum.
enum class GLOBAL_TEXTURE_TABLE_REGISTER : uint8
{
	t17, // ShadowMap1
	t18, // ShadowMap2
	t19, // ShadowMap3
	t20, // Reflection CubeMap
	END // Warning: Not an actual enum element. (Use only for checking the size of this enum.)
};

// TEMP
// Must set TRANSFORM_TABLE_DESC_COUNT same as the size of this enum.
enum class TRANSFORM_TABLE_REGISTER : uint8
{
	b3,
	END // Warning: Not an actual enum element. (Use only for checking the size of this enum.)
};

// Must set ANIMATION_TABLE_DESC_COUNT same as the size of this enum.
enum class ANIMATION_TABLE_REGISTER : uint8
{
	b4,
	END // Warning: Not an actual enum element. (Use only for checking the size of this enum.)
};

// NOTE: Do not put other than uint8-equivalent types.
union DescTableRegister
{
	uint8 value = 0;
	MATERIAL_TABLE_REGISTER materialRegister;
	PASS_TABLE_REGISTER passRegister;
	GLOBAL_TEXTURE_TABLE_REGISTER globalTextureRegister;
	TRANSFORM_TABLE_REGISTER transformRegister; // TEMP
	ANIMATION_TABLE_REGISTER animationRegister;
};

enum
{
	MATERIAL_TABLE_DESC_COUNT = static_cast<uint8>(MATERIAL_TABLE_REGISTER::END),
	MATERIAL_TABLE_SRV_COUNT = MATERIAL_TABLE_DESC_COUNT - static_cast<uint8>(MATERIAL_TABLE_REGISTER::t0),

	PASS_TABLE_DESC_COUNT = static_cast<uint8>(PASS_TABLE_REGISTER::END),
	GLOBAL_TEXTURE_TABLE_DESC_COUNT = static_cast<uint8>(GLOBAL_TEXTURE_TABLE_REGISTER::END),
	TRANSFORM_TABLE_DESC_COUNT = static_cast<uint8>(TRANSFORM_TABLE_REGISTER::END), // TEMP
	ANIMATION_TABLE_DESC_COUNT = static_cast<uint8>(ANIMATION_TABLE_REGISTER::END),
};

/******* Global Structs ********/
struct WindowInfo
{
	uint32 width;
	uint32 height;
	HWND hwnd;
	bool windowed;
};

struct Vertex
{
	Vec3 position;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;

	Vertex() {}

	Vertex(Vec3 position, Vec2 uv, Vec3 normal, Vec3 tangent)
		: position(position), uv(uv), normal(normal), tangent(tangent)
	{}
};

// Basic types
#include "Transform.h"

// TODO: Move to settings file.
const uint32 FRAME_RESOURCE_COUNT = 3;
const uint32 MAX_LIGHTS = 30;
const uint32 MAX_CAMERAS = 5;
const uint32 MAX_SHADOW_MAP_CAMERAS = 3;
const uint32 MAX_GAMEOBJECTS = 256;
const uint32 MAX_MATERIALS = 256;
const uint32 MAX_ANIMATION_SEQUENCES = 100;
const uint32 MAX_SKELETON_BONES = 96;