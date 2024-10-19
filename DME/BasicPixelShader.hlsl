#include "Common.hlsli"
#include "BlinnPhong.hlsli"

Texture2D g_texture0 : register(t0);		// texture
TextureCube g_textureCube0 : register(t1);	// diffuse map
TextureCube g_textureCube1 : register(t2);	// specular map

SamplerState g_sampler : register(s0);

cbuffer PixelConstantData : register(b0)
{
	float3 viewWorld;
	bool useTexture;
	Material material;
	Light light;
	int lightType;
	float3 padding;
};

float3 IBL(float3 normal, float3 viewDir, Material mat)
{
	// irradiance map 이용
	float3 diffuseIBL = mat.diffuse * g_textureCube0.Sample(g_sampler, normal).rgb;
	// radiance map 이용
	float3 specularIBL = mat.specular * g_textureCube1.Sample(g_sampler, reflect(-viewDir, normal)).rgb;
	
	return diffuseIBL + specularIBL;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 viewDir = normalize(viewWorld - input.posWorld);
	
	// 광원 타입에 따라 계산
	float3 BlinnPhongColor;
	if (lightType == 0)
	{
		BlinnPhongColor = ComputeDirectionalLight(light, material, input.normalWorld, viewDir);
	}
	else if (lightType == 1)
	{
		BlinnPhongColor = ComputePointLight(light, material, input.posWorld, input.normalWorld, viewDir);
	}
	else
	{
		BlinnPhongColor = ComputeSpotLight(light, material, input.posWorld, input.normalWorld, viewDir);
	}
	
	float3 IBLColor = IBL(input.normalWorld, viewDir, material);
	
	if (useTexture)
	{
		float4 diffuseSampled = g_texture0.Sample(g_sampler, input.texcoord);
		return float4(BlinnPhongColor + IBLColor, 1.0f) * diffuseSampled;
	}
	else
	{
		return float4(BlinnPhongColor + IBLColor, 1.0f);
	}
}