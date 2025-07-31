#ifndef _PARAMS_
#define _PARAMS_

// TODO: Get these define values from engine.
#define MAX_LIGHTS 30
#define MAX_CAMERAS 5
#define MAX_SHADOW_MAP_CAMERAS 3
#define MAX_SKELETON_BONES 96

struct LightInfo
{
    float3 color;
    int lightType;
    float3 position;
    float range;
    float3 direction;
    float angle;
    float fallOffStartRatio;
    float fallOffEndRatio;
    float2 __padding0;
};

struct MaterialColor // TODO: Move to somewhere
{
    float4 diffuseAlbedo;
    float3 specular;
    float roughness;
};

cbuffer PerMaterialConstants : register(b0)
{
	// If specular texture is set (i.e. g_tex_on_specular == 1), the specular will be overrided by that.
    float3 g_specular;
	
	// If roughness texture is set (i.e. g_tex_on_roughness == 1), the roughness will be overrided by that.
    float g_roughness;

	// If diffuse texture is set (i.e. g_tex_on_diffuse == 1), the diffuse will be overrided by the that.
	float4 g_diffuseColor;

	float g_matUScale;
	float g_matVScale;
	int g_tex_on_diffuse;
	int g_tex_on_specular;

	int g_tex_on_roughness;
	int g_tex_on_metallic;
	int g_tex_on_emissive;
	int g_tex_on_opacity;

	int g_tex_on_normal;
	int g_tex_on_worldPositionOffset;
	int g_tex_on_worldDisplacement;
	int g_tex_on_ambientOcclusion;

	int g_tex_on_tessellationMultiplier;
	int g_tex_on_refraction;
	int g_tex_on_misc;
	int g_texCube_on_skybox;
};

cbuffer PerPassConstants : register(b1)
{
	// NOTE: It can be either of a camera index or a shadow map camera index
	// depending on the type of current pass camera.
	uint g_currentCameraIdx;
    uint g_cameraProjectionType;
    float2 __passPadding0;
};

cbuffer GlobalConstants : register(b2)
{
	// For normal rendering cameras
	row_major matrix g_viewMatrices[MAX_CAMERAS];
	row_major matrix g_viewInvMatrices[MAX_CAMERAS];
	row_major matrix g_projMatrices[MAX_CAMERAS];
	row_major matrix g_viewProjMatrices[MAX_CAMERAS];

	// For shadow map cameras
	row_major matrix g_shadowCam_viewProjMatrices[MAX_SHADOW_MAP_CAMERAS];
    row_major matrix g_shadowMatrices[MAX_SHADOW_MAP_CAMERAS];
	
	float2 g_shadowCam1_PartitionRange;
	float2 g_shadowCam2_PartitionRange;
	float2 g_shadowCam3_PartitionRange;
	
	float g_totalTime;
	float g_deltaTime;

    uint g_lightCount;
    float3 g_ambientLightColor;
    LightInfo g_lights[MAX_LIGHTS];
    
	float g_terrainRowScale;
	float g_terrainColumnScale;
	float g_terrainHeightScale;    
	float __globalPadding0;
};

cbuffer PerGameObjectConstants : register(b3)
{
    row_major matrix g_worldMatrix;
    row_major matrix g_worldInvTransposeMatrix;
};

cbuffer PerAnimationConstants : register(b4)
{
	row_major matrix g_boneMatrices[MAX_SKELETON_BONES];
	row_major matrix g_boneInvTransposeMatrices[MAX_SKELETON_BONES];
};

Texture2D g_texture_diffuse : register(t0);
Texture2D g_texture_specular : register(t1);
Texture2D g_texture_roughness : register(t2);
Texture2D g_texture_metallic : register(t3);
Texture2D g_texture_emissive : register(t4);
Texture2D g_texture_opacity : register(t5);
Texture2D g_texture_normal : register(t6);
Texture2D g_texture_worldPositionOffset : register(t7);
Texture2D g_texture_worldDisplacement : register(t8);
Texture2D g_texture_ambientOcclusion : register(t9);
Texture2D g_texture_tessellationMultiplier : register(t10);
Texture2D g_texture_refraction : register(t11);
Texture2D<float2> g_texture_misc : register(t12);
TextureCube g_textureCube_skybox : register(t13);

Texture2D g_texture_shadowMap_1 : register(t17);
Texture2D g_texture_shadowMap_2 : register(t18);
Texture2D g_texture_shadowMap_3 : register(t19);
TextureCube g_textureCube_reflection : register(t20);

SamplerState g_sampler_pointWrap : register(s0);
SamplerState g_sampler_pointClamp : register(s1);
SamplerState g_sampler_linearWrap : register(s2);
SamplerState g_sampler_linearClamp : register(s3);
SamplerState g_sampler_anisotropicWrap : register(s4);
SamplerState g_sampler_anisotropicClamp : register(s5);

SamplerComparisonState g_comp_sampler_pointBorder : register(s6);

#endif // _PARAMS_