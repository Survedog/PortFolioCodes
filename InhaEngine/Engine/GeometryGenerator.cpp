#include "pch.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "HeightMap.h"

void GeometryGenerator::GetBoxMeshData(float width, float height, float depth, vector<Vertex>& outVertices, vector<uint32>& outIndices)
{
	const float hWidth = width / 2.f;
	const float hHeight = height / 2.f;
	const float hDepth = depth / 2.f;

	#pragma region Fill Vertex Data
	// Up face
	outVertices.emplace_back(Vec3(hWidth, hHeight, hDepth), Vec2(1.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f)); // Position, UV, Normal, Tangent
	outVertices.emplace_back(Vec3(-hWidth, hHeight, hDepth), Vec2(0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, hHeight, -hDepth), Vec2(0.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, hHeight, -hDepth), Vec2(1.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));

	// Down face
	outVertices.emplace_back(Vec3(hWidth, -hHeight, -hDepth), Vec2(1.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, -hDepth), Vec2(0.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, hDepth), Vec2(0.f, 1.f), Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, -hHeight, hDepth), Vec2(1.f, 1.f), Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f));

	// Right face
	outVertices.emplace_back(Vec3(hWidth, hHeight, hDepth), Vec2(1.f, 0.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f));
	outVertices.emplace_back(Vec3(hWidth, hHeight, -hDepth), Vec2(0.f, 0.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f));
	outVertices.emplace_back(Vec3(hWidth, -hHeight, -hDepth), Vec2(0.f, 1.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f));
	outVertices.emplace_back(Vec3(hWidth, -hHeight, hDepth), Vec2(1.f, 1.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f));

	// Left face
	outVertices.emplace_back(Vec3(-hWidth, hHeight, -hDepth), Vec2(1.f, 0.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, -1.f));
	outVertices.emplace_back(Vec3(-hWidth, hHeight, hDepth), Vec2(0.f, 0.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, -1.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, hDepth), Vec2(0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, -1.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, -hDepth), Vec2(1.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, -1.f));

	// Front face
	outVertices.emplace_back(Vec3(hWidth, hHeight, -hDepth), Vec2(1.f, 0.f), Vec3(0.f, 0.f, -1.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, hHeight, -hDepth), Vec2(0.f, 0.f), Vec3(0.f, 0.f, -1.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, -hDepth), Vec2(0.f, 1.f), Vec3(0.f, 0.f, -1.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, -hHeight, -hDepth), Vec2(1.f, 1.f), Vec3(0.f, 0.f, -1.f), Vec3(1.f, 0.f, 0.f));

	// Back face
	outVertices.emplace_back(Vec3(-hWidth, hHeight, hDepth), Vec2(1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, hHeight, hDepth), Vec2(0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, -hHeight, hDepth), Vec2(0.f, 1.f), Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, -hHeight, hDepth), Vec2(1.f, 1.f), Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f));

	#pragma endregion

	#pragma region Fill Index Data
	outIndices.assign({
		// Up face
		0, 3, 1,
		2, 1, 3,

		// Down face
		4, 7, 5,
		6, 5, 7,

		// Right face
		8, 11, 9,
		10, 9, 11,

		// Left face
		12, 15, 13,
		14, 13, 15,

		// Front face
		16, 19, 17,
		18, 17, 19,

		// Back face
		20, 23, 21,
		22, 21, 23 });
	#pragma endregion	
}

void GeometryGenerator::GetSphereMeshData(float radius, uint32 stackCount, uint32 sliceCount, vector<Vertex>& outVertices, vector<uint32>& outIndices)
{
	#pragma region Fill Vertex Data
	
	const float stackAngle = XM_PI / stackCount; // Angle between each stack
	const float sliceAngle = XM_2PI / sliceCount; // Angle between each slice

	for (uint32 stack = 0; stack <= stackCount; ++stack)
		for (uint32 slice = 0; slice <= sliceCount; ++slice)
		{
			float phi = slice * sliceAngle; // Angle around the stack
			float theta = stack * stackAngle; // Angle from the top

			const float sinTheta = sinf(theta);
			const float cosTheta = cosf(theta);
			const float sinPhi = sinf(phi);
			const float cosPhi = cosf(phi);

			float x = radius * sinTheta * cosPhi;
			float z = radius * sinTheta * sinPhi;
			float y = radius * cosTheta;

			float u = static_cast<float>(slice) / static_cast<float>(sliceCount);
			float v = static_cast<float>(stack) / static_cast<float>(stackCount);

			Vec3 normal = GetNormalizedVector(Vec3(x, y, z));
			Vec3 tangent = GetNormalizedVector(Vec3(x * cosPhi - y * sinPhi - x, x * sinPhi + y * cosPhi - y, 0)); // Tangent is in the direction of the slice

			// Vertex(Position, UV, Normal, Tangent)
			outVertices.emplace_back(Vec3(x, y, z), Vec2(u, v), normal, tangent);
		}

	#pragma endregion
	
	#pragma region Fill Index Data
	
	const uint32 numStackVertices = sliceCount + 1; // The amount of vertices for each stack.

	for (uint32 stack = 0; stack < stackCount; ++stack)
		for (uint32 slice = 0; slice < sliceCount; ++slice)
		{
			int vertex = stack * numStackVertices + slice;
			
			outIndices.push_back(vertex);
			outIndices.push_back(vertex + 1);
			outIndices.push_back(vertex + numStackVertices);

			outIndices.push_back(vertex + numStackVertices + 1);
			outIndices.push_back(vertex + numStackVertices);
			outIndices.push_back(vertex + 1);
		}

	#pragma endregion	
}

void GeometryGenerator::GetCapsuleMeshData(float radius, float halfHeight, uint32 halfStackCount, uint32 sliceCount, vector<Vertex>& outVertices, vector<uint32>& outIndices)
{
#pragma region Fill Vertex Data

	const float stackAngle = XM_PI / (halfStackCount * 2); // Angle between each stack
	const float sliceAngle = XM_2PI / sliceCount; // Angle between each slice

	// Left half sphere
	for (uint32 stack = 0; stack <= halfStackCount; ++stack)
		for (uint32 slice = 0; slice <= sliceCount; ++slice)
		{
			float phi = slice * sliceAngle; // Angle around the stack
			float theta = stack * stackAngle; // Angle from the top

			const float sinTheta = sinf(theta);
			const float cosTheta = cosf(theta);
			const float sinPhi = sinf(phi);
			const float cosPhi = cosf(phi);

			float x = -radius * cosTheta - halfHeight;
			float z = radius * sinTheta * sinPhi;
			float y = radius * sinTheta * cosPhi;

			float u = static_cast<float>(slice) / static_cast<float>(sliceCount);
			float v = static_cast<float>(stack) / static_cast<float>(halfStackCount * 2 + 1);

			Vec3 normal = GetNormalizedVector(Vec3(x, y, z));
			Vec3 tangent = GetNormalizedVector(Vec3(0, y * cosPhi - z * sinPhi - y, y * sinPhi + z * cosPhi - z)); // Tangent is in the direction of the slice

			// Vertex(Position, UV, Normal, Tangent)
			outVertices.emplace_back(Vec3(x, y, z), Vec2(u, v), normal, tangent);
		}

	// Right half sphere
	for (uint32 stack = halfStackCount; stack <= 2 * halfStackCount; ++stack)
		for (uint32 slice = 0; slice <= sliceCount; ++slice)
		{
			float phi = slice * sliceAngle; // Angle around the stack
			float theta = stack * stackAngle; // Angle from the top

			const float sinTheta = sinf(theta);
			const float cosTheta = cosf(theta);
			const float sinPhi = sinf(phi);
			const float cosPhi = cosf(phi);

			float x = -radius * cosTheta + halfHeight;
			float z = radius * sinTheta * sinPhi;
			float y = radius * sinTheta * cosPhi;

			float u = static_cast<float>(slice) / static_cast<float>(sliceCount);
			float v = static_cast<float>(stack + 1) / static_cast<float>(halfStackCount * 2 + 1);

			Vec3 normal = GetNormalizedVector(Vec3(x, y, z));
			Vec3 tangent = GetNormalizedVector(Vec3(0, y * cosPhi - z * sinPhi - y, y * sinPhi + z * cosPhi - z)); // Tangent is in the direction of the slice

			// Vertex(Position, UV, Normal, Tangent)
			outVertices.emplace_back(Vec3(x, y, z), Vec2(u, v), normal, tangent);
		}

#pragma endregion

#pragma region Fill Index Data

	const uint32 numStackVertices = sliceCount + 1; // The amount of vertices for each stack.

	for (uint32 stack = 0; stack <= 2 * halfStackCount; ++stack)
		for (uint32 slice = 0; slice < sliceCount; ++slice)
		{
			int vertex = stack * numStackVertices + slice;

			outIndices.push_back(vertex);
			outIndices.push_back(vertex + 1);
			outIndices.push_back(vertex + numStackVertices);

			outIndices.push_back(vertex + numStackVertices + 1);
			outIndices.push_back(vertex + numStackVertices);
			outIndices.push_back(vertex + 1);
		}

#pragma endregion	
}

void GeometryGenerator::GetPlaneMeshData(float width, float depth, vector<Vertex>& outVertices, vector<uint32>& outIndices)
{
	const float hWidth = width / 2.f;
	const float hDepth = depth / 2.f;

	outVertices.emplace_back(Vec3(hWidth, 0.f, hDepth), Vec2(1.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, 0.f, hDepth), Vec2(0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(-hWidth, 0.f, -hDepth), Vec2(0.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));
	outVertices.emplace_back(Vec3(hWidth, 0.f, -hDepth), Vec2(1.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f));

	outIndices.assign({
		0, 3, 1,
		2, 1, 3 });
}

void GeometryGenerator::GetHeightMapGridMeshData(uint32 rowCount, uint32 colCount, float rowScale, float colScale, vector<HeightMapVertex>& outVertices, vector<uint32>& outIndices)
{
	#pragma region Fill Vertex Data
	
	for (uint32 i = 0; i < rowCount + 1; ++i)
		for (uint32 j = 0; j < colCount + 1; ++j)
		{
			uint32 sampleIdx = i * colCount + j;

			// position
			const float z = j * colScale;
			const float x = i * rowScale;

			// uv
			const float u = static_cast<float>(j) / static_cast<float>(colCount);
			const float v = static_cast<float>(i) / static_cast<float>(rowCount);

			outVertices.emplace_back(Vec3(x, 0.f, z), Vec2(u, v));
		}

	#pragma endregion

	#pragma region Fill Index Data

	for (int i = 0; i < rowCount; ++i)
		for (int j = 0; j < colCount; ++j)
		{
			uint32 sampleIdx = i * (colCount + 1) + j;

			outIndices.push_back(sampleIdx);
			outIndices.push_back(sampleIdx + 1);
			outIndices.push_back(sampleIdx + colCount + 1);

			outIndices.push_back(sampleIdx + 1 + colCount + 1);
			outIndices.push_back(sampleIdx + colCount + 1);
			outIndices.push_back(sampleIdx + 1);
		}

	#pragma endregion
}

shared_ptr<Mesh> GeometryGenerator::CreateBoxMesh(float width, float height, float depth)
{
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>();

	vector<Vertex> vertices;
	vector<uint32> indices;
	GetBoxMeshData(width, height, depth, vertices, indices);

	boxMesh->Create(vertices, indices);
	return boxMesh;
}

shared_ptr<Mesh> GeometryGenerator::CreateSphereMesh(float radius, uint32 stackCount, uint32 sliceCount)
{
	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>();

	vector<Vertex> vertices;
	vector<uint32> indices;
	GetSphereMeshData(radius, stackCount, sliceCount, vertices, indices);

	sphereMesh->Create(vertices, indices);
	return sphereMesh;
}

shared_ptr<Mesh> GeometryGenerator::CreateCapsuleMesh(float radius, float halfHeight, uint32 stackCount, uint32 sliceCount)
{
	shared_ptr<Mesh> capsuleMesh = make_shared<Mesh>();

	vector<Vertex> vertices;
	vector<uint32> indices;
	GetCapsuleMeshData(radius, halfHeight, stackCount, sliceCount, vertices, indices);

	capsuleMesh->Create(vertices, indices);
	return capsuleMesh;
}

shared_ptr<Mesh> GeometryGenerator::CreatePlaneMesh(float width, float depth)
{
	shared_ptr<Mesh> planeMesh = make_shared<Mesh>();

	vector<Vertex> vertices;
	vector<uint32> indices;
	GetPlaneMeshData(width, depth, vertices, indices);

	planeMesh->Create(vertices, indices);
	return planeMesh;
}

shared_ptr<Mesh> GeometryGenerator::CreateHeightMapGridMesh(uint32 rowCount, uint32 colCount, float rowScale, float colScale)
{
	shared_ptr<Mesh> heightMapMesh = make_shared<Mesh>();

	vector<HeightMapVertex> vertices;
	vector<uint32> indices;
	GetHeightMapGridMeshData(rowCount, colCount, rowScale, colScale, vertices, indices);

	heightMapMesh->Create(reinterpret_cast<void*>(vertices.data()), static_cast<uint32>(vertices.size()), sizeof(HeightMapVertex), indices);
	return heightMapMesh;
}
