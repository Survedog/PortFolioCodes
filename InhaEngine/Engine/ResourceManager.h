#pragma once

#include "Material.h"
#include "Skeleton.h"
#include "SkeletalMesh.h"
#include "Shader.h"
#include "Texture.h"
#include "AnimationSequence.h"

class ResourceManager
{
	IMPL_SINGLETON(ResourceManager)

public:
	void Init();

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();

public:
	bool Add(const wstring& key, shared_ptr<Object> object)
	{		
		map<wstring, shared_ptr<Object>>& objectMap = _keyObjectMap[static_cast<uint8>(object->GetObjectType())];
		if (objectMap.find(key) != objectMap.end())
			return false;

		objectMap[key] = object;
		return true;
	}

	inline shared_ptr<Material> FindMaterial(const wstring& key)
	{
		shared_ptr<Object> object = Find(key, OBJECT_TYPE::MATERIAL);
		return object == nullptr ? nullptr : static_pointer_cast<Material>(object);
	}

	inline shared_ptr<Mesh> FindMesh(const wstring& key)
	{
		shared_ptr<Object> object = Find(key, OBJECT_TYPE::STATIC_MESH);
		return object == nullptr ? nullptr : static_pointer_cast<Mesh>(object);
	}

	inline shared_ptr<Skeleton> FindSkeleton(const wstring& key)
	{
		shared_ptr<Object> object = Find(key, OBJECT_TYPE::SKELETON);
		return object == nullptr ? nullptr : static_pointer_cast<Skeleton>(object);
	}

	inline shared_ptr<Shader> FindShader(const wstring& key)
	{
		shared_ptr<Object> object = Find(key, OBJECT_TYPE::SHADER);
		return object == nullptr ? nullptr : static_pointer_cast<Shader>(object);
	}

	inline shared_ptr<Texture> FindTexture(const wstring& key)
	{
		shared_ptr<Object> object = Find(key, OBJECT_TYPE::TEXTURE);
		return object == nullptr ? nullptr : static_pointer_cast<Texture>(object);
	}

	inline shared_ptr<Material> LoadMaterial(const wstring& key, const wstring& path)
	{
		shared_ptr<Material> object = Load<Material>(key, path);
		return object == nullptr ? nullptr : object;
	}

	inline shared_ptr<Mesh> LoadMesh(const wstring& key, const wstring& path)
	{
		shared_ptr<Mesh> object = Load<Mesh>(key, path);
		return object == nullptr ? nullptr : object;
	}

	inline shared_ptr<Skeleton> LoadSkeleton(const wstring& key, const wstring& path)
	{
		shared_ptr<Skeleton> object = Load<Skeleton>(key, path);
		return object == nullptr ? nullptr : object;
	}

	inline shared_ptr<Shader> LoadShader(const wstring& key, const wstring& path)
	{
		shared_ptr<Shader> object = Load<Shader>(key, path);
		return object == nullptr ? nullptr : object;
	}

	inline shared_ptr<Texture> LoadTexture(const wstring& key, const wstring& path)
	{
		shared_ptr<Texture> object = Load<Texture>(key, path);
		return object == nullptr ? nullptr : object;
	}	

private:
	shared_ptr<Object> Find(const wstring& key, OBJECT_TYPE type)
	{
		map<wstring, shared_ptr<Object>>& objectMap = _keyObjectMap[static_cast<uint8>(type)];
		auto iter = objectMap.find(key);
		if (iter != objectMap.end())
			return iter->second;
		return nullptr;
	}

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path)
	{
		static_assert(is_base_of_v<Object, T>);

		map<wstring, shared_ptr<Object>>& objectMap = _keyObjectMap[static_cast<uint8>(Object::GetObjectType<T>())];
		auto iter = objectMap.find(key);
		if (iter != objectMap.end())
		{
			// TODO: Put warning message.
			return static_pointer_cast<T>(iter->second);
		}
		
		shared_ptr<T> object = make_shared<T>();
		object->Load(path);
		objectMap[key] = object;
		return object;
	}

private:
	map<wstring, shared_ptr<Object>> _keyObjectMap[OBJECT_TYPE_COUNT];

// FBX
public:
	inline shared_ptr<fbx::FbxManager> GetFbxManager() const { return _fbxManager; }

	void ImportFbxFileData(const wstring& fbxFilePath, vector<shared_ptr<Mesh>>& outMeshes, vector<shared_ptr<AnimationSequence>>& outAnimSequences);

private:
	void InitFbxManager();
	fbx::FbxScene* LoadFbxScene(const wstring& fbxFilePath);
	
	/* Mesh */
	// Create static or skeletal mesh, skeleton(if skeletal mesh), material, animation sequences using fbxNode with mesh attribute.
	// If the node has a eSkin type deformer, then the mesh is assumed as a skeletal mesh.
	shared_ptr<Mesh> GetResourcesFromFbxMeshNode(fbx::FbxNode* fbxNode, const wstring& meshName, vector<shared_ptr<AnimationSequence>>& outAnimSequences);

	
	// Get Tangent data of mesh. Depending on the tagent mapping mode, the index of the output tangent vector can be either of vertex id(control point index) or index id(polygon vertex index).
	void GetTangentData(fbx::FbxMesh* fbxMesh, vector<fbx::FbxVector4>& outTangents, fbx::FbxGeometryElement::EMappingMode& outTangentMappingMode, uint32 vertexCount, uint32 indexCount);

	void GetPolygonMaterialData(fbx::FbxMesh* fbxMesh, FbxArray<int32>& outPolygonMaterialIndices);
	void GetSubMeshesInfo(fbx::FbxMesh* fbxMesh, vector<SubMeshInfo>& outSubMeshesInfo, const FbxArray<int32>& polygonSubMeshIndices);	
	
	void GetVerticesAndIndices(fbx::FbxMesh* fbxMesh,
		vector<Vertex>& outVertices,
		vector<uint32>& outIndices,
		const vector<SubMeshInfo>& subMeshesInfo,
		const FbxArray<int32>& polygonSubMeshIndices,
		const fbx::FbxArray<fbx::FbxVector4>& normalPerIndex,
		const fbx::FbxArray<fbx::FbxVector2>& uvPerIndex,
		const vector<fbx::FbxVector4>& tangents,
		fbx::FbxGeometryElement::EMappingMode tangentMappingMode);

	void GetSkinnedVerticesAndIndices(fbx::FbxMesh* fbxMesh,
		shared_ptr<Skeleton> skeleton,
		vector<SkinnedVertex>& outVertices,
		vector<uint32>& outIndices,
		const vector<SubMeshInfo>& subMeshesInfo,
		const FbxArray<int32>& polygonSubMeshIndices,
		const fbx::FbxArray<fbx::FbxVector4>& normalPerIndex,
		const fbx::FbxArray<fbx::FbxVector2>& uvPerIndex,
		const vector<fbx::FbxVector4>& tangents,
		fbx::FbxGeometryElement::EMappingMode tangentMappingMode);

	/* Skeleton */
	shared_ptr<Skeleton> CreateSkeletonFromFbxNode(fbx::FbxNode* fbxNode, const wstring& skeletonName);
	shared_ptr<Bone> CreateBoneHierarchyFromFbxNode(fbx::FbxNode* fbxNode, const wstring& boneName);
	
	// Get the root node of the skeleton tree that the input node is in.
	fbx::FbxNode* GetSkeletonTreeRootNode(fbx::FbxNode* fbxSkeletonNode);

	// Get skeleton node that is linked with the fbxMesh's first skin cluster.
	fbx::FbxNode* GetFirstClusterLinkNode(fbx::FbxNode* fbxMeshNode);

	/* Material */
	shared_ptr<Material> CreateMaterialFromFbxMaterial(fbx::FbxSurfaceMaterial* fbxMaterial, const wstring& materialName, bool isSkinned);

	/* Animation */
	shared_ptr<AnimationSequence> CreateAnimSequenceFromFbx(fbx::FbxAnimStack* fbxAnimStack, fbx::FbxNode* skeletonRootNode, shared_ptr<Skeleton> targetSkeleton, const wstring& animStackName);
	ANIMKEY_INTERPOLATION_MODE GetInterpolationFromFbxInterpolationType(fbx::FbxAnimCurveDef::EInterpolationType fbxInterpolationType);	
	
	// NOTE: The keyframe values are in the parent bone space (not based on the bind pose transform of the bone).
	// Also, use CreateRotationFCurvesFromFbx() for rotation.
	shared_ptr<AnimationFCurve> CreateFCurveFromFbx(fbx::FbxAnimCurve* fbxAnimCurve);

	// Fills outFCurves with AnimationFCurve objects for quaternion rotation in XYZW order. outFCurves will be empty if it fails.
	// If FbxAnimStack is set, it will create animation curves contained in the FbxAnimStack. If not, will create default animation curves.
	// The created fcurve's keyframes are made by calling FbxNode::EvaluateLocalTransform for each frames of the FbxAnimCurve in first channel,
	// therefore each fcurves' details will not preserved.
	// The keyframe values are in the bind pose space of the bone. (Unlike CreateFCurveFromFbx())
	void CreateRotationFCurvesFromFbx(fbx::FbxNode* skeletonNode, vector<shared_ptr<AnimationFCurve>>& outFCurves, fbx::FbxAnimStack* fbxAnimStack = nullptr);

	// Create BoneAnimation objects according to skeleton-attribute-attached fbxNode hierarchy.
	// If fbxAnimLayer argument is set, when searching fcurves, it will try to use animation curve nodes that is contained in the anim layer.
	// If the animation curve nodes are not found, it will behave the same as when fbxAnimLayer is not set. (use directly-attached curve nodes)
	void CreateBoneAnimationsFromFbx(fbx::FbxNode* skeletonNode, uint32& boneId, vector<shared_ptr<BoneAnimation>>& outBoneAnimations, fbx::FbxAnimLayer* fbxAnimLayer = nullptr);

	/* Commons */
	// Read element values from the FbxGeometryElement type object.
	// NOTE: The size of outElementValues is determined by the mapping mode of the FbxGeometryElement input.
	// If the mapping mode is eByControlPoint, the size will be equal to the number of control points (vertices).
	// Or if the mapping mode is eByPolygonVertex, the size will be equal to the number of polygon vertices (indices).
	template<typename T>
	void ReadGeometryElements(const fbx::FbxLayerElementTemplate<T>* elements, vector<T>& outElementValues, uint32 vertexCount, uint32 indexCount)
	{
		if (elements == nullptr)
		{
			// TODO: Print error log. "Geometry element is null."
			return;
		}

		outElementValues.clear();

		if (elements->GetMappingMode() == fbx::FbxLayerElement::eByControlPoint) // Elements are mapped to control points
		{
			outElementValues.resize(vertexCount);

			if (elements->GetReferenceMode() == fbx::FbxGeometryElement::eDirect)
			{
				assert(vertexCount == elements->GetDirectArray().GetCount());
				for (int vertexId = 0; vertexId < elements->GetDirectArray().GetCount(); ++vertexId)
					outElementValues[vertexId] = elements->GetDirectArray().GetAt(vertexId);
			}
			else if (elements->GetReferenceMode() == fbx::FbxGeometryElement::eIndexToDirect)
			{
				assert(vertexCount == elements->GetIndexArray().GetCount());
				for (int vertexId = 0; vertexId < elements->GetIndexArray().GetCount(); ++vertexId)
				{
					int index = elements->GetIndexArray().GetAt(vertexId);
					outElementValues[vertexId] = elements->GetDirectArray().GetAt(index);
				}
			}
			else
			{
				//TODO: Print error log. "Unsupported geometry element reference mode."
				return;
			}
		}
		else if (elements->GetMappingMode() == fbx::FbxLayerElement::eByPolygonVertex) // Elements are mapped to polygon vertices (indices)
		{
			outElementValues.resize(indexCount);

			if (elements->GetReferenceMode() == fbx::FbxGeometryElement::eDirect)
			{
				assert(indexCount == elements->GetDirectArray().GetCount());
				for (int indexId = 0; indexId < elements->GetDirectArray().GetCount(); ++indexId)
					outElementValues[indexId] = elements->GetDirectArray().GetAt(indexId);
			}
			else if (elements->GetReferenceMode() == fbx::FbxGeometryElement::eIndexToDirect)
			{
				assert(indexCount == elements->GetIndexArray().GetCount());
				for (int indexId = 0; indexId < elements->GetIndexArray().GetCount(); ++indexId)
					outElementValues[indexId] = elements->GetDirectArray().GetAt(elements->GetIndexArray().GetAt(indexId));
			}
			else
			{
				//TODO: Print error log. "Unsupported geometry element reference mode."
				return;
			}
		}
		else
		{
			//TODO: Print error log. "Unsupported geometry element mapping mode."
			return;
		}
	}

private:
	shared_ptr<fbx::FbxManager> _fbxManager = nullptr;
};

