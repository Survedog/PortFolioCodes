#pragma once

class Mesh;
class HeightMap;
union HeightMapSample;
struct HeightMapVertex;

class GeometryGenerator
{
public:
	static void GetBoxMeshData(float width, float height, float depth, vector<Vertex>& outVertices, vector<uint32>& outIndices);
	static void GetSphereMeshData(float radius, uint32 stackCount, uint32 sliceCount, vector<Vertex>& outVertices, vector<uint32>& outIndices);
	static void GetCapsuleMeshData(float radius, float halfHeight, uint32 halfStackCount, uint32 sliceCount, vector<Vertex>& outVertices, vector<uint32>& outIndices);
	static void GetPlaneMeshData(float width, float depth, vector<Vertex>& outVertices, vector<uint32>& outIndices);
	static void GetHeightMapGridMeshData(uint32 rowCount, uint32 colCount, float rowScale, float colScale, vector<HeightMapVertex>& outVertices, vector<uint32>& outIndices);

	static shared_ptr<Mesh> CreateBoxMesh(float width, float height, float depth);
	static shared_ptr<Mesh> CreateSphereMesh(float radius, uint32 stackCount = 8, uint32 sliceCount = 16);
	static shared_ptr<Mesh> CreateCapsuleMesh(float radius, float halfHeight, uint32 halfStackCount = 4, uint32 sliceCount = 16);
	static shared_ptr<Mesh> CreatePlaneMesh(float width, float depth);
	static shared_ptr<Mesh> CreateHeightMapGridMesh(uint32 rowCount, uint32 colCount, float rowScale, float colScale);
};

