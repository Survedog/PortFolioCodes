#include "pch.h"
#include "ResourceManager.h"
#include "SwapChainManager.h"
#include "Engine.h"

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();

	InitFbxManager();
}

void ResourceManager::CreateDefaultShader()
{
	vector<D3D12_INPUT_ELEMENT_DESC> defaultInputElements = {
		//{ SemanticName,	SemanticIndex,	Format,							InputSlot,	AlignedByteOffset,	InputSlotClass,								InstanceDataStepRate }
		  { "POSITION",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			0,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "TEXCOORD",		0,				DXGI_FORMAT_R32G32_FLOAT,		0,			12,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "NORMAL",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			20,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "TANGENT",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			32,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
	};

	vector<D3D12_INPUT_ELEMENT_DESC> terrainInputElements = {
		//{ SemanticName,	SemanticIndex,	Format,							InputSlot,	AlignedByteOffset,	InputSlotClass,								InstanceDataStepRate }
		  { "POSITION",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			0,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "TEXCOORD",		0,				DXGI_FORMAT_R32G32_FLOAT,		0,			12,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
	};

	vector<D3D12_INPUT_ELEMENT_DESC> skinnedMeshInputElements = {
		//{ SemanticName,	SemanticIndex,	Format,							InputSlot,	AlignedByteOffset,	InputSlotClass,								InstanceDataStepRate }
		  { "POSITION",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			0,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "TEXCOORD",		0,				DXGI_FORMAT_R32G32_FLOAT,		0,			12,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "NORMAL",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			20,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "TANGENT",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			32,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "WEIGHTS",		0,				DXGI_FORMAT_R32G32B32_FLOAT,	0,			44,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
		  { "BONEINDICES",	0,				DXGI_FORMAT_R8G8B8A8_UINT,		0,			56,					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0					 },
	};

	{
		shared_ptr<Shader> defaultShader = make_shared<Shader>();
		defaultShader->SetName(L"Shader_default");

		ShaderInfo shaderInfo;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		shaderInfo.sampleDesc.Count = ENGINE->GetSwapChainManager()->GetMSAASampleCount();
		shaderInfo.sampleDesc.Quality = ENGINE->GetSwapChainManager()->GetMSAAQualityLevelCount() - 1;

		defaultShader->Create(L"..\\Engine\\Resources\\Shader\\forward.hlsl", shaderInfo, ShaderArgument(), defaultInputElements);
		Add(defaultShader->GetName(), defaultShader);
	}

	{
		shared_ptr<Shader> tessLODShader = make_shared<Shader>();
		tessLODShader->SetName(L"Shader_tessLOD");

		ShaderInfo shaderInfo;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.topology = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		shaderInfo.sampleDesc.Count = ENGINE->GetSwapChainManager()->GetMSAASampleCount();
		shaderInfo.sampleDesc.Quality = ENGINE->GetSwapChainManager()->GetMSAAQualityLevelCount() - 1;

		ShaderArgument shaderArgs;
		shaderArgs.hsName = L"HS";
		shaderArgs.dsName = L"DS";

		tessLODShader->Create(L"..\\Engine\\Resources\\Shader\\forwardTessLOD.hlsl", shaderInfo, shaderArgs, defaultInputElements);
		Add(tessLODShader->GetName(), tessLODShader);
	}

	{
		shared_ptr<Shader> shadowMapShader = make_shared<Shader>();
		shadowMapShader->SetName(L"Shader_shadowMap");

		ShaderInfo shaderInfo;
		shaderInfo.shaderType = SHADER_TYPE::SHADOW;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		shaderInfo.sampleDesc.Count = 1;
		shaderInfo.sampleDesc.Quality = 0;

		shadowMapShader->Create(L"..\\Engine\\Resources\\Shader\\shadowMap.hlsl", shaderInfo, ShaderArgument(), defaultInputElements);
		Add(shadowMapShader->GetName(), shadowMapShader);
	}

	{
		shared_ptr<Shader> skyboxShader = make_shared<Shader>();
		skyboxShader->SetName(L"Shader_skybox");

		ShaderInfo shaderInfo;
		shaderInfo.shaderType = SHADER_TYPE::FORWARD;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE;
		shaderInfo.sampleDesc.Count = ENGINE->GetSwapChainManager()->GetMSAASampleCount();
		shaderInfo.sampleDesc.Quality = ENGINE->GetSwapChainManager()->GetMSAAQualityLevelCount() - 1;

		skyboxShader->Create(L"..\\Engine\\Resources\\Shader\\skybox.hlsl", shaderInfo, ShaderArgument(), defaultInputElements);
		Add(skyboxShader->GetName(), skyboxShader);
	}

	{
		shared_ptr<Shader> terrainShader = make_shared<Shader>();
		terrainShader->SetName(L"Shader_terrain");

		ShaderInfo shaderInfo;
		shaderInfo.shaderType = SHADER_TYPE::FORWARD;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.topology = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		shaderInfo.sampleDesc.Count = ENGINE->GetSwapChainManager()->GetMSAASampleCount();
		shaderInfo.sampleDesc.Quality = ENGINE->GetSwapChainManager()->GetMSAAQualityLevelCount() - 1;

		ShaderArgument shaderArgs;
		shaderArgs.hsName = L"HS";
		shaderArgs.dsName = L"DS";

		terrainShader->Create(L"..\\Engine\\Resources\\Shader\\terrain.hlsl", shaderInfo, shaderArgs, terrainInputElements);
		Add(terrainShader->GetName(), terrainShader);
	}

	{
		shared_ptr<Shader> skinnedMeshShader = make_shared<Shader>();
		skinnedMeshShader->SetName(L"Shader_skinnedMesh");

		ShaderInfo shaderInfo;
		shaderInfo.rasterizerType = RASTERIZER_TYPE::CULL_BACK;
		shaderInfo.blendType = BLEND_TYPE::DEFAULT;
		shaderInfo.depthStencilType = DEPTH_STENCIL_TYPE::LESS;
		shaderInfo.sampleDesc.Count = ENGINE->GetSwapChainManager()->GetMSAASampleCount();
		shaderInfo.sampleDesc.Quality = ENGINE->GetSwapChainManager()->GetMSAAQualityLevelCount() - 1;

		ShaderArgument shaderArgs;
		shaderArgs.vsDefines.emplace_back(L"SKINNED");

		skinnedMeshShader->Create(L"..\\Engine\\Resources\\Shader\\forward.hlsl", shaderInfo, shaderArgs, skinnedMeshInputElements);
		Add(skinnedMeshShader->GetName(), skinnedMeshShader);
	}
}

void ResourceManager::CreateDefaultMaterial()
{
	{
		shared_ptr<Material> defaultLODMaterial = make_shared<Material>();
		defaultLODMaterial->SetName(L"Material_tessLOD");
		defaultLODMaterial->SetShader(FindShader(L"Shader_tessLOD"));

		shared_ptr<Texture> texture = LoadTexture(L"T_DefaultGrid", L"..\\Engine\\Resources\\Texture\\DefaultGrid.png");
		defaultLODMaterial->SetDiffuseTexture(texture);
		defaultLODMaterial->SetSpecular(Vec3(0.3f));
		defaultLODMaterial->SetRoughness(0.5f);

		Add(defaultLODMaterial->GetName(), defaultLODMaterial);
	}

	{
		shared_ptr<Material> defaultMaterial = make_shared<Material>();
		defaultMaterial->SetName(L"Material_default");
		defaultMaterial->SetShader(FindShader(L"Shader_default"));

		shared_ptr<Texture> texture = LoadTexture(L"T_DefaultGrid", L"..\\Engine\\Resources\\Texture\\DefaultGrid.png");
		defaultMaterial->SetDiffuseTexture(texture);
		defaultMaterial->SetSpecular(Vec3(0.3f));
		defaultMaterial->SetRoughness(0.5f);

		Add(defaultMaterial->GetName(), defaultMaterial);
	}

	{
		shared_ptr<Material> shadowMapMaterial = make_shared<Material>();
		shadowMapMaterial->SetName(L"Material_shadowMap");
		shadowMapMaterial->SetShader(FindShader(L"Shader_shadowMap"));

		Add(shadowMapMaterial->GetName(), shadowMapMaterial);
	}

	{
		shared_ptr<Material> skyboxMaterial = make_shared<Material>();
		skyboxMaterial->SetName(L"Material_skybox");
		skyboxMaterial->SetShader(FindShader(L"Shader_skybox"));

		shared_ptr<Texture> textureCube = LoadTexture(L"T_SkyboxCubeMap", L"..\\Engine\\Resources\\Texture\\skyboxCubeMap.dds");
		skyboxMaterial->SetTextureCube(textureCube);

		Add(skyboxMaterial->GetName(), skyboxMaterial);
	}
}

// TODO: Implement animation importing
void ResourceManager::ImportFbxFileData(const wstring& fbxFilePath, vector<shared_ptr<Mesh>>& outMeshes, vector<shared_ptr<AnimationSequence>>& outAnimSequences)
{
	outMeshes.clear();	

	fbx::FbxScene* fbxScene = LoadFbxScene(fbxFilePath);
	if (fbxScene == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	// Iterate through all scene nodes and extract scene data (meshes & skeletons).
	queue<fbx::FbxNode*> nodeQueue;
	nodeQueue.push(fbxScene->GetRootNode());

	while (!nodeQueue.empty())
	{
		fbx::FbxNode* node = nodeQueue.front(); nodeQueue.pop();

		if (node != nullptr && node->GetNodeAttribute() != nullptr)
		{
			if (node->GetNodeAttribute()->GetAttributeType() == fbx::FbxNodeAttribute::eMesh)
			{
				shared_ptr<Mesh> mesh = GetResourcesFromFbxMeshNode(node, L"Mesh_" + AnsiToWString(node->GetName()), outAnimSequences);
				if (mesh != nullptr) outMeshes.push_back(mesh);
			}
		}

		// Add all child nodes to the node queue.
		for (int32 i = 0; i < node->GetChildCount(); ++i)
		{
			fbx::FbxNode* childNode = node->GetChild(i);
			if (childNode != nullptr) nodeQueue.push(childNode);
		}
	}

	// Register created meshes and materials to resource manager.
	for (shared_ptr<Mesh>& mesh : outMeshes)
	{
		mesh->SetName(L"Mesh_" + mesh->GetName());
		Add(mesh->GetName(), mesh);

		for (shared_ptr<Material>& material : mesh->GetMaterials())
		{
			material->SetName(L"Material_" + material->GetName());
			Add(material->GetName(), material);
		}
	}	

	fbxScene->Destroy();
}


void ResourceManager::InitFbxManager()
{
	_fbxManager.reset(fbx::FbxManager::Create(), [] (fbx::FbxManager* p) {
		if (p != nullptr) p->Destroy();
	});

	if (_fbxManager == nullptr)
	{
		THROW_EXCEPTION(L"Unable to create FBX Manager.");
		return;
	}

	fbx::FbxIOSettings* ioSettings = fbx::FbxIOSettings::Create(_fbxManager.get(), IOSROOT);
	if (ioSettings == nullptr)
	{
		THROW_EXCEPTION(L"Unable to create  IO Settings.");
		return;
	}

	_fbxManager->SetIOSettings(ioSettings);
}

fbx::FbxScene* ResourceManager::LoadFbxScene(const wstring& fbxFilePath)
{
	if (!fs::exists(fbxFilePath)) {
		// TODO: Print Error log. "Error: The input fbx file does not exist."
		return nullptr;
	}

	fbx::FbxScene* scene = fbx::FbxScene::Create(_fbxManager.get(), "ImportedScene");
	if (scene == nullptr) {
		// TODO: Print Error log. "Error: Unable to create FBX Scene."
		return nullptr;
	}

	// Create FbxImporter to import the fbx scene data.
	fbx::FbxImporter* importer = fbx::FbxImporter::Create(GetFbxManager().get(), "");
	if (importer == nullptr) {
		// TODO: Print Error log. "Error: Unable to create FBX Importer."
		return nullptr;
	}

	if (!importer->Initialize(WStringToAnsi(fbxFilePath).c_str(), -1, GetFbxManager()->GetIOSettings()))
	{
		// TODO: Print Error log. "Error: Unable to initialize FBX Importer with file: " + fbxFilePath + L"\nReason: " + AnsiToWString(importer->GetStatus().GetErrorString());
		return nullptr;
	}

	// Populate scene with the imported data.
	if (!importer->Import(scene))
	{
		// TODO: Print Error log. "Error: Unable to import FBX file: " + fbxFilePath + L"\nReason: " + AnsiToWString(importer->GetStatus().GetErrorString());
		return nullptr;
	}
	importer->Destroy(); // The importer is not needed after populating the scene.

	// Convert scene to match DirectX coordinate(axis) system.
	//fbx::FbxAxisSystem::DirectX.ConvertScene(scene);

	// Triangulate all meshes that can be triangulated.
	FbxGeometryConverter geometryConverter(GetFbxManager().get());
	geometryConverter.Triangulate(scene, true);

	return scene;
}

void ResourceManager::GetTangentData(fbx::FbxMesh* fbxMesh, vector<fbx::FbxVector4>& outTangents, fbx::FbxGeometryElement::EMappingMode& outTangentMappingMode, uint32 vertexCount, uint32 indexCount)
{
	if (fbxMesh == nullptr)
	{
		//TODO: Print warning log.
		return;
	}

	// Generate tangent information if the mesh doesn't contain tangent information.
	assert(fbxMesh->GetElementTangentCount() > 0 || fbxMesh->GenerateTangentsData(0));
	
	fbx::FbxGeometryElementTangent* tangentElement = fbxMesh->GetElementTangent();
	outTangentMappingMode = tangentElement->GetMappingMode();
	ReadGeometryElements(tangentElement, outTangents, vertexCount, indexCount);
}

void ResourceManager::GetPolygonMaterialData(fbx::FbxMesh* fbxMesh, FbxArray<int32>& outPolygonMaterialIndices)
{
	if (fbxMesh == nullptr)
	{
		//TODO: Print warning log.
		return;
	}

	if (fbxMesh->GetElementMaterialCount() > 0)
	{
		fbx::FbxGeometryElementMaterial* materialElement = fbxMesh->GetElementMaterial();
		assert(materialElement->GetMappingMode() == fbx::FbxLayerElement::eByPolygon);
		//outPolygonMaterialIndices.Resize(fbxMesh->GetPolygonCount());
		materialElement->GetIndexArray().CopyTo(outPolygonMaterialIndices);
	}
}

void ResourceManager::GetSubMeshesInfo(fbx::FbxMesh* fbxMesh, vector<SubMeshInfo>& outSubMeshInfos, const FbxArray<int32>& polygonSubMeshIndices)
{
	if (fbxMesh == nullptr)
	{
		//TODO: Print warning log.
		return;
	}

	// Get the amount of submeshes.
	uint32 subMeshCount = 0;
	assert(polygonSubMeshIndices.Size() == fbxMesh->GetPolygonCount() && "Polygon sub mesh index vector's size is different from the fbxMesh's polygon count");
	for (int32 poly = 0; poly < fbxMesh->GetPolygonCount(); ++poly)
		subMeshCount = max(subMeshCount, polygonSubMeshIndices[poly] + 1);

	vector<uint32> indexCount(subMeshCount);
	vector<int32> baseVertexLocations(subMeshCount);
	vector<uint32> startIndexLocations(subMeshCount);

	for (int32 poly = 0; poly < fbxMesh->GetPolygonCount(); ++poly)
	{
		int32 subMeshIdx = polygonSubMeshIndices[poly];
		
		indexCount[subMeshIdx] += fbxMesh->GetPolygonSize(poly);

		// Get the base vertex location for each submesh.
		for (int32 v = 0; v < fbxMesh->GetPolygonSize(poly); ++v)
			baseVertexLocations[subMeshIdx] = min(baseVertexLocations[subMeshIdx], fbxMesh->GetPolygonVertex(poly, v));
	}

	for (uint32 i = 1; i < subMeshCount; ++i)
		startIndexLocations[i] = startIndexLocations[i - 1] + indexCount[i - 1];

	outSubMeshInfos.reserve(subMeshCount);
	for (uint32 i = 0; i < subMeshCount; ++i)
		outSubMeshInfos.emplace_back(indexCount[i], baseVertexLocations[i], startIndexLocations[i], i);
}

void ResourceManager::GetVerticesAndIndices(fbx::FbxMesh* fbxMesh, 
	vector<Vertex>& outVertices,
	vector<uint32>& outIndices,
	const vector<SubMeshInfo>& subMeshesInfo,
	const FbxArray<int32>& polygonSubMeshIndices,
	const fbx::FbxArray<fbx::FbxVector4>& normalPerIndex,
	const fbx::FbxArray<fbx::FbxVector2>& uvPerIndex,
	const vector<fbx::FbxVector4>& tangents,
	fbx::FbxGeometryElement::EMappingMode tangentMappingMode)
{
	if (fbxMesh == nullptr)
	{
		// TODO: Print warning log.
		return;
	}

	uint32 subMeshCount = static_cast<uint32>(subMeshesInfo.size());

	vector<vector<uint32>> subMeshIndices(subMeshCount);
	for (int32 i = 0; i < subMeshCount; ++i)
		subMeshIndices[i].reserve(subMeshesInfo[i].indexCount);

	for (int32 poly = 0; poly < fbxMesh->GetPolygonCount(); ++poly)
	{
		int32 subMeshIdx = polygonSubMeshIndices[poly];

		for (int32 v = 0; v < fbxMesh->GetPolygonSize(poly); ++v)
		{
			int32 vertexId = fbxMesh->GetPolygonVertex(poly, v);
			int32 indexId = fbxMesh->GetPolygonVertexIndex(poly) + v;

			fbx::FbxVector4 position = fbxMesh->GetControlPoints()[vertexId];
			fbx::FbxVector4 tangent = tangents.empty() ? FbxVector4(0.f, 0.f, 0.f, 0.f) :
				(tangentMappingMode == fbx::FbxGeometryElement::eByControlPoint) ? tangents[vertexId] : tangents[indexId];

			subMeshIndices[subMeshIdx].push_back(vertexId - subMeshesInfo[subMeshIdx].baseVertexLocation);
			outVertices[vertexId] = Vertex(
				Vec3(position[0], position[1], position[2]), // Position
				Vec2(uvPerIndex[indexId][0], 1.f - uvPerIndex[indexId][1]), // UV. FBX's uv coordinate origin(0, 0) is on the bottom-left corner, so it needs to be upside down.
				Vec3(normalPerIndex[indexId][0], normalPerIndex[indexId][1], normalPerIndex[indexId][2]), // Normal
				Vec3(tangent[0], tangent[1], tangent[2]) // Tangent
			);

			uint32 rearrangedIndexId = subMeshesInfo[subMeshIdx].startIndexLocation + static_cast<uint32>(subMeshIndices[subMeshIdx].size()) - 1;
			outIndices[rearrangedIndexId] = subMeshIndices[subMeshIdx].back();
		}
	}
}

void ResourceManager::GetSkinnedVerticesAndIndices(fbx::FbxMesh* fbxMesh,
	shared_ptr<Skeleton> skeleton,
	vector<SkinnedVertex>& outVertices,
	vector<uint32>& outIndices,
	const vector<SubMeshInfo>& subMeshesInfo,
	const FbxArray<int32>& polygonSubMeshIndices,
	const fbx::FbxArray<fbx::FbxVector4>& normalPerIndex,
	const fbx::FbxArray<fbx::FbxVector2>& uvPerIndex,
	const vector<fbx::FbxVector4>& tangents,
	fbx::FbxGeometryElement::EMappingMode tangentMappingMode)
{
	if (fbxMesh == nullptr || skeleton == nullptr)
	{
		// TODO: Print warning log.
		return;
	}

	// TODO: Resize outVertices and outIndices here.

	uint32 subMeshCount = static_cast<uint32>(subMeshesInfo.size());

	vector<vector<uint32>> subMeshIndices(subMeshCount);
	for (int32 i = 0; i < subMeshCount; ++i)
		subMeshIndices[i].reserve(subMeshesInfo[i].indexCount);

	// Construct vertices and indices.
	for (int32 poly = 0; poly < fbxMesh->GetPolygonCount(); ++poly)
	{
		int32 subMeshIdx = polygonSubMeshIndices[poly];

		for (int32 v = 0; v < fbxMesh->GetPolygonSize(poly); ++v)
		{
			int32 vertexId = fbxMesh->GetPolygonVertex(poly, v);
			int32 indexId = fbxMesh->GetPolygonVertexIndex(poly) + v;

			fbx::FbxVector4 position = fbxMesh->GetControlPoints()[vertexId];
			fbx::FbxVector4 tangent = tangents.empty() ? FbxVector4(0.f, 0.f, 0.f, 0.f) :
				(tangentMappingMode == fbx::FbxGeometryElement::eByControlPoint) ? tangents[vertexId] : tangents[indexId];

			subMeshIndices[subMeshIdx].push_back(vertexId - subMeshesInfo[subMeshIdx].baseVertexLocation);
			outVertices[vertexId] = SkinnedVertex(
				Vec3(position[0], position[1], position[2]), // Position
				Vec2(uvPerIndex[indexId][0], 1.f - uvPerIndex[indexId][1]), // UV. FBX's uv coordinate origin(0, 0) is on the bottom-left corner, so it needs to be upside down.
				Vec3(normalPerIndex[indexId][0], normalPerIndex[indexId][1], normalPerIndex[indexId][2]), // Normal
				Vec3(tangent[0], tangent[1], tangent[2]) // Tangent
			);

			uint32 rearrangedIndexId = subMeshesInfo[subMeshIdx].startIndexLocation + static_cast<uint32>(subMeshIndices[subMeshIdx].size()) - 1;
			outIndices[rearrangedIndexId] = subMeshIndices[subMeshIdx].back();
		}
	}

	// Set skinning info.
	vector<Vec4> vertexBoneWeights(outVertices.size(), Vec4::Zero);

	for (int32 deformerIdx = 0; deformerIdx < fbxMesh->GetDeformerCount(); ++deformerIdx)
	{
		if (fbxMesh->GetDeformer(deformerIdx)->GetDeformerType() != fbx::FbxDeformer::eSkin)
			continue;

		fbx::FbxSkin* fbxSkin = fbx::FbxCast<FbxSkin>(fbxMesh->GetDeformer(deformerIdx));
		assert(fbxSkin != nullptr);
		
		for (int32 clusterIdx = 0; clusterIdx < fbxSkin->GetClusterCount(); ++clusterIdx)
		{
			fbx::FbxCluster* cluster = fbxSkin->GetCluster(clusterIdx);
			assert(cluster != nullptr);

			wstring linkedBoneName = AnsiToWString(cluster->GetLink()->GetName());
			int32 linkedBoneId = skeleton->FindBoneIdByName(linkedBoneName);

			int32* skinnedVerticesId = cluster->GetControlPointIndices();
			double* vertexWeights = cluster->GetControlPointWeights();
			if (skinnedVerticesId == nullptr || vertexWeights == nullptr) continue;

			for (int32 i = 0; i < cluster->GetControlPointIndicesCount(); ++i)
			{
				uint32 vertexId = skinnedVerticesId[i];
				float weight = static_cast<float>(vertexWeights[i]);
				
				// Sets a vertex's linked bones id and weights.
				for (uint32 j = 0; j < 4; ++j)
					if (!outVertices[vertexId].HasNthLinkedBone(j))
					{
						outVertices[vertexId].linkedBonesId[j] = static_cast<BYTE>(linkedBoneId);
						SetVectorElementByIdx(vertexBoneWeights[vertexId], weight, j);
						break;
					}
			}
		}
	}

	// Make the sum of the weights one.
	for (size_t i = 0; i < outVertices.size(); ++i)
	{
		float weightSum = GetVectorSum(vertexBoneWeights[i]);
		if (weightSum == 0.f) assert(false);

		vertexBoneWeights[i] /= weightSum;
		outVertices[i].weights = Vec3(vertexBoneWeights[i].x, vertexBoneWeights[i].y, vertexBoneWeights[i].z); // Fourth weight is 1 - (the sum of other three weights).
	}
}

shared_ptr<Mesh> ResourceManager::GetResourcesFromFbxMeshNode(fbx::FbxNode* fbxNode, const wstring& meshName, vector<shared_ptr<AnimationSequence>>& outAnimSequences)
{
	fbx::FbxMesh* fbxMesh = fbxNode->GetMesh();
	if (fbxMesh == nullptr) {
		// TODO: Print error log.
		return nullptr;
	}
	
	uint32 vertexCount = fbxMesh->GetControlPointsCount();
	uint32 indexCount = fbxMesh->GetPolygonVertexCount();
	
	// Get normal vector for each polygon vertex(index).
	fbx::FbxArray<fbx::FbxVector4> normals;
	fbxMesh->GetPolygonVertexNormals(normals);

	// Get UV for each polygon vertex(index).
	fbx::FbxArray<fbx::FbxVector2> uvs;
	{
		fbx::FbxStringList uvSetNames;
		fbxMesh->GetUVSetNames(uvSetNames);
		assert(uvSetNames.GetCount() > 0);
		fbxMesh->GetPolygonVertexUVs(uvSetNames[0], uvs);
	}	

	// Get tangent info.
	vector<fbx::FbxVector4> tangents;	
	fbx::FbxGeometryElement::EMappingMode tangentMappingMode;
	GetTangentData(fbxMesh, tangents, tangentMappingMode, vertexCount, indexCount);

	// Get material index for each polygon.
	FbxArray<int32> polygonMaterialIndices;
	GetPolygonMaterialData(fbxMesh, polygonMaterialIndices);

	// Get the amount, index count, base vertex location, start index location of submeshes.
	uint32 subMeshCount = 0;
	vector<SubMeshInfo> subMeshesInfo;
	GetSubMeshesInfo(fbxMesh, subMeshesInfo, polygonMaterialIndices); // polygon's material index is the same as submesh index.

	assert(normals.Size() == indexCount && uvs.Size() == indexCount &&
	(tangents.size() == indexCount || tangents.size() == vertexCount),
	polygonMaterialIndices.Size() == fbxMesh->GetPolygonCount());	

	// Get skeleton info.
	fbx::FbxNode* skeletonRootNode = GetSkeletonTreeRootNode(GetFirstClusterLinkNode(fbxNode));
	shared_ptr<Skeleton> skeleton = CreateSkeletonFromFbxNode(skeletonRootNode, AnsiToWString(skeletonRootNode->GetName()));

	shared_ptr<Mesh> mesh = nullptr;
	if (skeleton == nullptr) // Create static mesh.
	{
		// Create vertex and index array for each submesh.
		vector<Vertex> vertices(vertexCount);
		vector<uint32> indices(indexCount);
		GetVerticesAndIndices(fbxMesh, vertices, indices, subMeshesInfo, polygonMaterialIndices, normals, uvs, tangents, tangentMappingMode);

		// Create mesh object.
		mesh = make_shared<Mesh>();
		mesh->Create(vertices, indices, nullptr, true);
		mesh->SetName(meshName);
	}
	else
	{
		// Create skeletal mesh.
		vector<SkinnedVertex> vertices(vertexCount);
		vector<uint32> indices(indexCount);
		GetSkinnedVerticesAndIndices(fbxMesh, skeleton, vertices, indices, subMeshesInfo, polygonMaterialIndices, normals, uvs, tangents, tangentMappingMode);

		shared_ptr<SkeletalMesh> skeletalMesh = make_shared<SkeletalMesh>(skeleton);
		skeletalMesh->Create(vertices, indices, nullptr, true);
		skeletalMesh->SetName(meshName);
		mesh = skeletalMesh;

		// Create animation sequences.
		for (int32 i = 0; i < fbxNode->GetScene()->GetSrcObjectCount<FbxAnimStack>(); ++i)
		{
			FbxAnimStack* animStack = fbxNode->GetScene()->GetSrcObject<FbxAnimStack>();
			assert(animStack != nullptr);
			shared_ptr<AnimationSequence> animSequence = CreateAnimSequenceFromFbx(animStack, skeletonRootNode, skeleton, AnsiToWString(animStack->GetName()));
			outAnimSequences.push_back(animSequence);
		}
	}

	for (int i = 0; i < static_cast<uint32>(subMeshesInfo.size()); ++i)
		mesh->AddSubMeshInfo(L"SubMesh_" + i, subMeshesInfo[i]); // TODO: Put submesh's actual name. 
	
	// Create materials according to fbx materials and add them to the mesh.
	bool isSkinned = (skeleton != nullptr);
	for (int32 i = 0; i < fbxNode->GetMaterialCount(); ++i)
	{
		FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i);
		shared_ptr<Material> material = CreateMaterialFromFbxMaterial(fbxMaterial, L"Material_" + AnsiToWString(fbxMaterial->GetName()), isSkinned);
		if (material == nullptr) continue;

		mesh->AddMaterial(material);
	}
	
	return mesh;
}

shared_ptr<Skeleton> ResourceManager::CreateSkeletonFromFbxNode(fbx::FbxNode* fbxNode, const wstring& skeletonName)
{
	if (fbxNode == nullptr || fbxNode->GetSkeleton() == nullptr)
	{
		// TODO: Print error log.
		return nullptr;
	}

	shared_ptr<Bone> rootBone = CreateBoneHierarchyFromFbxNode(fbxNode, AnsiToWString(fbxNode->GetName()));
	shared_ptr<Skeleton> skeleton = make_shared<Skeleton>();
	skeleton->SetRootBone(rootBone);
	return skeleton;
}

// Iterate children fbx nodes to find bone parts and construct 1the whole skeleton.
shared_ptr<Bone> ResourceManager::CreateBoneHierarchyFromFbxNode(fbx::FbxNode* fbxNode, const wstring& boneName)
{
	if (fbxNode == nullptr || fbxNode->GetSkeleton() == nullptr)
	{
		// TODO: Print error log.
		return nullptr;
	}

	fbx::FbxSkeleton* fbxSkeleton = fbxNode->GetSkeleton();

	FbxAMatrix& transformMatrix = fbxNode->EvaluateLocalTransform(FBXSDK_TIME_INFINITE);
	const FbxVector4& localPosition = transformMatrix.GetT();
	const FbxQuaternion& localQuatRotation = transformMatrix.GetQ();
	const FbxVector4& localEulerRotation = transformMatrix.GetR();
	const FbxVector4& localScale = transformMatrix.GetS();

	Transform boneTransform;
	boneTransform.position = GetGameCoordsFromDXCoords(Vec3(localPosition[0], localPosition[1], localPosition[2]));
	boneTransform.rotation = Quaternion(localQuatRotation[0], localQuatRotation[1], localQuatRotation[2], localQuatRotation[3]);
	boneTransform.scale = GetGameCoordsFromDXCoords(Vec3(localScale[0], localScale[1], localScale[2]));

	float boneLength = fbxSkeleton->Size.Get(); // TODO: Size vs LimbLength, which one is correct?
	shared_ptr<Bone> bone = make_shared<Bone>(boneName, boneTransform, boneLength, nullptr);

	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		shared_ptr<Bone> childBone = CreateBoneHierarchyFromFbxNode(fbxNode->GetChild(i), AnsiToWString(fbxNode->GetChild(i)->GetName()));
		bone->children.push_back(childBone);
		childBone->parent = bone;
	}

	return bone;
}

fbx::FbxNode* ResourceManager::GetSkeletonTreeRootNode(fbx::FbxNode* fbxSkeletonNode)
{
	if (fbxSkeletonNode == nullptr || fbxSkeletonNode->GetSkeleton() == nullptr)
	{
		// TODO: Print warning log.
		return nullptr;
	}

	fbx::FbxNode* currentNode = fbxSkeletonNode;
	while (currentNode->GetParent() != nullptr && currentNode->GetParent()->GetSkeleton())
		currentNode = currentNode->GetParent();
	return currentNode;
}

fbx::FbxNode* ResourceManager::GetFirstClusterLinkNode(fbx::FbxNode* fbxMeshNode)
{
	if (fbxMeshNode == nullptr || fbxMeshNode->GetMesh() == nullptr)
	{
		// TODO: Print warning log.
		return nullptr;
	}

	fbx::FbxMesh* fbxMesh = fbxMeshNode->GetMesh();
	for (int deformerIdx = 0; deformerIdx < fbxMesh->GetDeformerCount(); ++deformerIdx)
	{
		if (fbxMesh->GetDeformer(deformerIdx)->GetDeformerType() != fbx::FbxDeformer::eSkin)
			continue;

		fbx::FbxSkin* fbxSkin = fbx::FbxCast<fbx::FbxSkin>(fbxMesh->GetDeformer(deformerIdx));
		assert(fbxSkin != nullptr);

		for (int clusterIdx = 0; clusterIdx < fbxSkin->GetClusterCount(); ++clusterIdx)
		{
			fbx::FbxCluster* cluster = fbxSkin->GetCluster(clusterIdx);
			if (cluster->GetLink() != nullptr) return cluster->GetLink();
		}
	}
	return nullptr;
}

shared_ptr<Material> ResourceManager::CreateMaterialFromFbxMaterial(fbx::FbxSurfaceMaterial* fbxMaterial, const wstring& materialName, bool isSkinned)
{
	if (fbxMaterial == nullptr)
	{
		// TODO: Print error log.
		return nullptr;
	}

	fbx::FbxSurfacePhong* fbxPhongMaterial = fbx::FbxCast<fbx::FbxSurfacePhong>(fbxMaterial);
	if (fbxPhongMaterial == nullptr)
	{
		//TODO: Print error log. "Material is not a Phong material, unsupported material type."
		return nullptr;
	}

	shared_ptr<Material> material = make_shared<Material>();
	material->SetName(materialName);
	
	// Get material properties from the FbxSurfacePhong material.
	fbx::FbxDouble3 diffuse = fbxPhongMaterial->Diffuse.Get();
	fbx::FbxDouble3 specular = fbxPhongMaterial->Specular.Get();
	fbx::FbxDouble shininess = fbxPhongMaterial->Shininess.Get() / 100.f;
	assert(shininess >= 0.f && shininess <= 1.f);

	material->SetDiffuseColor(Vec4(diffuse[0], diffuse[1], diffuse[2], 1.f));
	material->SetSpecular(Vec3(specular[0], specular[1], specular[2]));
	material->SetRoughness(1.f - shininess);

	// Get texture properties from the FbxSurfacePhong material.
#pragma region Get Texture Properties
	if (fbxPhongMaterial->Diffuse.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* diffuseTexture = fbxPhongMaterial->Diffuse.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(diffuseTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetDiffuseTexture(LoadTexture(textureName, filePath));
	}

	if (fbxPhongMaterial->Specular.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* specularTexture = fbxPhongMaterial->Specular.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(specularTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetSpecularTexture(LoadTexture(textureName, filePath));
	}

	if (fbxPhongMaterial->Shininess.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* shininessTexture = fbxPhongMaterial->Shininess.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(shininessTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetRoughnessTexture(LoadTexture(textureName, filePath)); // TODO: Need to be inverted for roughness.
	}

	if (fbxPhongMaterial->Reflection.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* reflectionTexture = fbxPhongMaterial->Reflection.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(reflectionTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetMetallicTexture(LoadTexture(textureName, filePath));
	}

	if (fbxPhongMaterial->Emissive.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* emissiveTexture = fbxPhongMaterial->Emissive.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(emissiveTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetEmissiveTexture(LoadTexture(textureName, filePath));
	}

	if (fbxPhongMaterial->TransparencyFactor.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* transparencyTexture = fbxPhongMaterial->TransparencyFactor.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(transparencyTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetOpacityTexture(LoadTexture(textureName, filePath)); // TODO: Need to be inverted for opacity.
	}

	if (fbxPhongMaterial->Ambient.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* ambientTexture = fbxPhongMaterial->Ambient.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(ambientTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetAmbientOcclusionTexture(LoadTexture(textureName, filePath));
	}

	if (fbxPhongMaterial->NormalMap.GetSrcObjectCount() > 0)
	{
		FbxFileTexture* normalMapTexture = fbxPhongMaterial->NormalMap.GetSrcObject<FbxFileTexture>(0);
		wstring filePath = AnsiToWString(normalMapTexture->GetFileName());
		wstring textureName = L"Texture_" + AnsiToWString(fs::path(filePath).filename().string());
		material->SetNormalTexture(LoadTexture(textureName, filePath));
	}
#pragma endregion

	if (isSkinned)
		material->SetShader(FindShader(L"Shader_skinnedMesh"));
	else
		material->SetShader(FindShader(L"Shader_default"));
	return material;
}

shared_ptr<AnimationSequence> ResourceManager::CreateAnimSequenceFromFbx(fbx::FbxAnimStack* fbxAnimStack, fbx::FbxNode* skeletonRootNode, shared_ptr<Skeleton> targetSkeleton, const wstring& animStackName)
{
	if (fbxAnimStack == nullptr || skeletonRootNode == nullptr) {
		// TODO: Print error log.
		return nullptr;
	}
	
	shared_ptr<AnimationSequence> animSequence = make_shared<AnimationSequence>();	

	vector<shared_ptr<BoneAnimation>> boneAnimations;
	for (int32 i = 0; i < fbxAnimStack->GetMemberCount<FbxAnimLayer>(); ++i)
	{
		uint32 boneId = 0;
		fbx::FbxAnimLayer* animLayer = fbxAnimStack->GetMember<FbxAnimLayer>(i);
		string name = animLayer->GetName(); // TEMP
		CreateBoneAnimationsFromFbx(skeletonRootNode, boneId, boneAnimations, animLayer);
		assert(boneId == static_cast<uint32>(targetSkeleton->GetBones().size() - 1));
	}

	animSequence->SetTargetSkeleton(targetSkeleton);
	for (const shared_ptr<BoneAnimation>& boneAnim : boneAnimations)
		if (boneAnim != nullptr)
			animSequence->SetBoneAnimation(boneAnim);

	return animSequence;
}

void ResourceManager::CreateBoneAnimationsFromFbx(fbx::FbxNode* skeletonNode, uint32& boneId, vector<shared_ptr<BoneAnimation>>& outBoneAnimations, fbx::FbxAnimLayer* fbxAnimLayer)
{
	if (fbxAnimLayer == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	shared_ptr<BoneAnimation> boneAnimation = make_shared<BoneAnimation>(boneId);
	fbx::FbxAMatrix bindTransformMatrix = skeletonNode->EvaluateLocalTransform();

	// Translation
	if (skeletonNode->LclTranslation.IsAnimated())
	{		
		shared_ptr<AnimationFCurve> translationFCurve[3];

		fbx::FbxAnimCurveNode* fbxTranslationCurveNode = skeletonNode->LclTranslation.GetCurveNode(fbxAnimLayer);
		if (fbxTranslationCurveNode == nullptr) fbxTranslationCurveNode = skeletonNode->LclTranslation.GetCurveNode();

		if (fbxTranslationCurveNode != nullptr)
		{
			fbx::FbxVector4 boneBindPosition = bindTransformMatrix.GetT();
			for (int32 channel = 0; channel < fbxTranslationCurveNode->GetChannelsCount(); ++channel)
			{
				fbx::FbxAnimCurve* fbxTranslationCurve = fbxTranslationCurveNode->GetCurve(channel);
				if (fbxTranslationCurve != nullptr) translationFCurve[channel] = CreateFCurveFromFbx(fbxTranslationCurve);

				// Subtract the bind position from the translation keyframes to get the local translation.
				for (AnimationKeyframe& keyframe : translationFCurve[channel]->GetKeyframes())
				{
					keyframe.value -= boneBindPosition[channel];
				}
			}
			// Convert from directX coordinate system (Z-forward Y-up left handed) to game coordinate system (X-forward Z-up left handed)
			boneAnimation->SetChannelFCurve(translationFCurve[0], ANIMATION_BONE_CHANNEL::LOCATION_Y);
			boneAnimation->SetChannelFCurve(translationFCurve[1], ANIMATION_BONE_CHANNEL::LOCATION_Z);
			boneAnimation->SetChannelFCurve(translationFCurve[2], ANIMATION_BONE_CHANNEL::LOCATION_X);
		}
	}

	// Quaternion Rotation
	if (skeletonNode->LclRotation.IsAnimated())
	{

		fbx::FbxAnimCurveNode* fbxRotationCurveNode = skeletonNode->LclRotation.GetCurveNode(fbxAnimLayer);
		if (fbxRotationCurveNode == nullptr) fbxRotationCurveNode = skeletonNode->LclRotation.GetCurveNode();

		if (fbxRotationCurveNode != nullptr)
		{
			vector<shared_ptr<AnimationFCurve>> rotationFCurves;
			CreateRotationFCurvesFromFbx(skeletonNode, rotationFCurves);
			
			if (static_cast<uint32>(rotationFCurves.size()) == 4)
			{
				boneAnimation->SetChannelFCurve(rotationFCurves[0], ANIMATION_BONE_CHANNEL::QUAT_ROTATION_X);
				boneAnimation->SetChannelFCurve(rotationFCurves[1], ANIMATION_BONE_CHANNEL::QUAT_ROTATION_Y);
				boneAnimation->SetChannelFCurve(rotationFCurves[2], ANIMATION_BONE_CHANNEL::QUAT_ROTATION_Z);
				boneAnimation->SetChannelFCurve(rotationFCurves[3], ANIMATION_BONE_CHANNEL::QUAT_ROTATION_W);
			}
		}
	}

	// Scale
	if (skeletonNode->LclScaling.IsAnimated())
	{
		shared_ptr<AnimationFCurve> scaleFCurve[3];

		fbx::FbxAnimCurveNode* fbxScaleCurveNode = skeletonNode->LclScaling.GetCurveNode(fbxAnimLayer);
		if (fbxScaleCurveNode == nullptr) fbxScaleCurveNode = skeletonNode->LclScaling.GetCurveNode();

		if (fbxScaleCurveNode != nullptr)
		{
			fbx::FbxVector4 boneBindScale = bindTransformMatrix.GetS();

			for (int32 channel = 0; channel < fbxScaleCurveNode->GetChannelsCount(); ++channel)
			{
				fbx::FbxAnimCurve* fbxScaleCurve = fbxScaleCurveNode->GetCurve(channel);
				if (fbxScaleCurve != nullptr) scaleFCurve[channel] = CreateFCurveFromFbx(fbxScaleCurve);

				// Divide animation scale by the bone bind scale to get local scale.
				for (AnimationKeyframe& keyframe : scaleFCurve[channel]->GetKeyframes())
				{
					keyframe.value /= boneBindScale[channel];
				}
			}
			// Convert from directX coordinate system (Z-forward Y-up left handed) to game coordinate system (X-forward Z-up left handed)
			boneAnimation->SetChannelFCurve(scaleFCurve[0], ANIMATION_BONE_CHANNEL::SCALE_Y);
			boneAnimation->SetChannelFCurve(scaleFCurve[1], ANIMATION_BONE_CHANNEL::SCALE_Z);
			boneAnimation->SetChannelFCurve(scaleFCurve[2], ANIMATION_BONE_CHANNEL::SCALE_X);
		}
	}

	if (!boneAnimation->IsEmpty()) outBoneAnimations.push_back(boneAnimation);

	for (int32 i = 0; i < skeletonNode->GetChildCount(); ++i)
	{
		fbx::FbxNode* childNode = skeletonNode->GetChild(i);
		boneId++;
		CreateBoneAnimationsFromFbx(childNode, boneId, outBoneAnimations, fbxAnimLayer);
	}
}

ANIMKEY_INTERPOLATION_MODE ResourceManager::GetInterpolationFromFbxInterpolationType(fbx::FbxAnimCurveDef::EInterpolationType fbxInterpolationType)
{
	switch (fbxInterpolationType)
	{
	case fbx::FbxAnimCurveDef::eInterpolationConstant:
		return ANIMKEY_INTERPOLATION_MODE::CONSTANT;

	case fbx::FbxAnimCurveDef::eInterpolationLinear:
		return ANIMKEY_INTERPOLATION_MODE::LINEAR;

	case fbx::FbxAnimCurveDef::eInterpolationCubic:
		return ANIMKEY_INTERPOLATION_MODE::CUBIC;
	}

	assert(false && "Unknown interpolation type.");
	return ANIMKEY_INTERPOLATION_MODE();
}

shared_ptr<AnimationFCurve> ResourceManager::CreateFCurveFromFbx(fbx::FbxAnimCurve* fbxAnimCurve)
{
	if (fbxAnimCurve == nullptr)
	{
		// TODO: Print error log.
		return nullptr;
	}

	shared_ptr<AnimationFCurve> animFCurve = make_shared<AnimationFCurve>();
	for (uint32 keyIdx = 0; keyIdx < fbxAnimCurve->KeyGetCount(); ++keyIdx)
	{		
		fbx::FbxAnimCurveKey fbxKey = fbxAnimCurve->KeyGet(keyIdx);
		int32 frame = fbxKey.GetTime().GetFrameCount(); // TODO: Apply global setting time mode 
		float value = fbxKey.GetValue();

		ANIMKEY_INTERPOLATION_MODE interpolationMode = GetInterpolationFromFbxInterpolationType(fbxAnimCurve->KeyGetInterpolation(keyIdx));
		animFCurve->InsertKeyframe(AnimationKeyframe(frame, value, interpolationMode));
	}

	return animFCurve;
}

void ResourceManager::CreateRotationFCurvesFromFbx(fbx::FbxNode* skeletonNode, vector<shared_ptr<AnimationFCurve>>& outFCurves, fbx::FbxAnimStack* fbxAnimStack)
{
	outFCurves.clear();
	if (skeletonNode == nullptr)
	{
		// TODO: Print error log.
		return;
	}
	
	fbx::FbxAnimCurveNode* fbxCurveNode;
	if (fbxAnimStack == nullptr) fbxCurveNode = skeletonNode->LclRotation.GetCurveNode();
	else fbxCurveNode = skeletonNode->LclRotation.GetCurveNode(fbxAnimStack);

	if (fbxCurveNode == nullptr) return;

	assert(fbxCurveNode->GetChannelsCount() > 0 && fbxCurveNode->GetCurveCount(0) > 0);

	outFCurves.resize(4, nullptr); // X, Y, Z, W
	for (int32 i = 0; i < 4; ++i)
		outFCurves[i] = make_shared<AnimationFCurve>();

	// Get every time that has a animation key.
	set<fbx::FbxTime> keyTimes;
	for (uint32 channelIdx = 0; channelIdx < fbxCurveNode->GetChannelsCount(); ++channelIdx)
	{
		fbx::FbxString channelName = fbxCurveNode->GetChannelName(channelIdx);
		fbx::FbxAnimCurve* fbxAnimCurve = fbxCurveNode->GetCurve(channelIdx);
		for (uint32 keyIdx = 0; keyIdx < fbxAnimCurve->KeyGetCount(); ++keyIdx)
		{
			keyTimes.insert(fbxAnimCurve->KeyGet(keyIdx).GetTime());
		}
	}

	fbx::FbxQuaternion boneInverseBindRotation = skeletonNode->EvaluateLocalTransform(FBXSDK_TIME_INFINITE).GetQ();
	boneInverseBindRotation.Inverse();

	// Evaluate rotation value for each keyframe time and insert the keyframes made of them to outFCurves.
	for (const fbx::FbxTime& time : keyTimes)
	{
		int32 frame = time.GetFrameCount(); // TODO: Apply global setting time mode 
		fbx::FbxAMatrix transformMatrix = skeletonNode->EvaluateLocalTransform(time);
		fbx::FbxQuaternion quatRotation = transformMatrix.GetQ();

		// Convert the rotation value to local rotation for bone bind space.
		quatRotation = boneInverseBindRotation * quatRotation;

		for (int32 i = 0; i < 4; ++i)
			outFCurves[i]->InsertKeyframe(AnimationKeyframe(frame, quatRotation[i], ANIMKEY_INTERPOLATION_MODE::CUBIC));
	}
}
