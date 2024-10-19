#include "Common.hlsli"
#include "BlinnPhong.hlsli"

Texture2D g_texture0 : register(t0);
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

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 viewDir = normalize(viewWorld - input.posWorld);
	
	float4 diffuseSampled = g_texture0.Sample(g_sampler, input.texcoord);
	
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
	
	return useTexture ? float4(BlinnPhongColor, 1.0) * diffuseSampled : float4(BlinnPhongColor, 1.0);
}