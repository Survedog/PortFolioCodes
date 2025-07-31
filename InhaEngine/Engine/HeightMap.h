#pragma once

#include "Object.h"

class Texture;

struct HeightMapVertex
{
	Vec3 position;
	Vec2 uv;

	HeightMapVertex() = default;

	HeightMapVertex(Vec3 position, Vec2 uv)
		: position(position), uv(uv)
	{
	}
};

// Height map sample(point) data for physics scene.
union HeightMapSample
{
public:
	uint32 value;

private:
	struct
	{
		int16 height; // Height value
		uint8 materialIndex0; // Material0 index for the height sample.
		uint8 materialIndex1; // Material1 index for the height sample.
	} s;

public:
	int16 GetHeight() const { return s.height; }
	uint8 GetMaterialIndex0() const { return s.materialIndex0 & static_cast<uint8>(0x7F); }
	uint8 GetMaterialIndex1() const { return s.materialIndex1 & static_cast<uint8>(0x7F); }

	inline void SetHeight(int16 newHeight) { s.height = newHeight; }
	
	// Material index must be between 0 and 127.
	inline void SetMaterialIndex0(uint8 newIdx) { s.materialIndex0 = static_cast<uint8>((s.materialIndex0 & 0xFE) + (newIdx & 0x01)); }
	
	// Material index must be between 0 and 127.
	inline void SetMaterialIndex1(uint8 newIdx) { s.materialIndex1 = static_cast<uint8>((s.materialIndex1 & 0xFE) + (newIdx & 0x01)); }

	inline void SetTessFlag() { s.materialIndex0 |= static_cast<uint8>(0x01); }
	inline void ClearTessFlag() { s.materialIndex0 &= static_cast<uint8>(0xFE); }
};

class HeightMap : public Object
{
public:
	HeightMap();
	HeightMap(shared_ptr<Texture> heightMapTexture);
	virtual ~HeightMap() = default;

public:
	void GetPhysicsHeightFieldDataFromTexture(vector<HeightMapSample>& outData) const;

	inline size_t GetRowCount() const { return _rowCount; }
	inline size_t GetColumnCount() const { return _columnCount; }

	inline float GetRowScale() const { return _rowScale; }
	inline float GetColumnScale() const { return _columnScale; }
	inline float GetHeightScale() const { return _heightScale; }

	shared_ptr<Texture> GetHeightMapTexture() { return _heightMapTexture; }
	shared_ptr<const Texture> GetHeightMapTexture() const { return _heightMapTexture; }

	void SetHeightMapTexture(shared_ptr<Texture> texture);

private:
	// Pack 1byte numbers into 4byte number (follow little endian).
	inline uint32 PackByteStrided32BitValue(uint8* startPtr) const { return (startPtr[3] << 24) + (startPtr[2] << 16) + (startPtr[1] << 8) + startPtr[0]; }

private:
	uint32 _rowCount = 0;
	uint32 _columnCount = 0;
	
	float _rowScale = 10.f;
	float _columnScale = 10.f;
	float _heightScale = 1.f / 64.f;

	shared_ptr<Texture> _heightMapTexture;
};

