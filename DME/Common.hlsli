#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

struct VertexShaderInput
{
	float3 posModel : POSITION;
	float3 normalModel : NORMAL0;
	float2 texcoord : TEXCOORD;
	float3 faceNormal : NORMAL1;
};

struct PixelShaderInput
{
	float4 posProj : SV_POSITION;	// projection space
	float3 posWorld : POSITION;		// world space
	float3 normalWorld : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct CubeMappingPixelShaderInput
{
	float4 posProj : SV_POSITION;
	float3 posModel : POSITION;
};

#endif // __COMMON_HLSLI__