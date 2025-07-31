#include "pch.h"
#include "HeightMap.h"
#include "Engine.h"
#include "Texture.h"
#include "ResourceManager.h"

HeightMap::HeightMap() :
	Object(OBJECT_TYPE::HEIGHT_MAP, L"HeightMap_NoName"), _heightMapTexture(nullptr)
{
}

HeightMap::HeightMap(shared_ptr<Texture> heightMapTexture) :
	Object(OBJECT_TYPE::HEIGHT_MAP, L"HeightMap_NoName"), _heightMapTexture(nullptr)
{
	SetHeightMapTexture(heightMapTexture);
}

void HeightMap::GetPhysicsHeightFieldDataFromTexture(vector<HeightMapSample>& outData) const
{
	if (GetHeightMapTexture() == nullptr)
	{
		// TODO: Print error log.
		outData.clear();
		return;
	}

	bool isHeightUnsigned = GetHeightMapTexture()->GetMetadata().format == DXGI_FORMAT_R16G16_UNORM;

	vector<uint32> pixelData;
	GetHeightMapTexture()->ReadBackTextureData(pixelData);
	const uint32 pixelByteSize = sizeof(uint32); // R16G16_UNORM format has 2 channels, each 16 bits (2 bytes) wide.

	outData.clear();
	outData.resize(GetRowCount() * GetColumnCount());
	assert(outData.size() == pixelData.size() && "The heightmap texture size is different from the height map size.");

	for (size_t i = 0; i < GetRowCount(); ++i)
		for (size_t j = 0; j < GetColumnCount(); ++j)
		{
			uint32 sampleIdx = i * GetColumnCount() + j;
			uint32 pixelValue = pixelData[sampleIdx];
			outData[sampleIdx].value = pixelValue;

			// Convert unsigned value to signed value by shifting its bits once to the right. (Some details will be removed.)
			// The heights from UNORM heightmap image will be cut down to half because of it.
			if (isHeightUnsigned)
			{
				int16 height = outData[sampleIdx].GetHeight() >> 1;
				outData[sampleIdx].SetHeight(height);
			}
		}
}

void HeightMap::SetHeightMapTexture(shared_ptr<Texture> texture)
{
	_heightMapTexture = texture;
	if (texture != nullptr)
	{
		if (texture->GetMetadata().format != DXGI_FORMAT_R16G16_UNORM)
		{
			// TODO: Put warning message "The format of height map DDS file must be 32bit R16G16 UNORM type."
			return;
		}
		_rowCount = texture->GetHeight();
		_columnCount = texture->GetWidth();
	}
}
