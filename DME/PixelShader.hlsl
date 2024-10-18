Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

struct PixelShaderInput
{
	float4 posProj : SV_POSITION; // projection space
	float3 posWorld : POSITION; // world space
	float3 normalWorld : NORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer PixelConstantData : register(b0)
{
	bool useTexture;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	if (useTexture)
	{
		return g_texture0.Sample(g_sampler, input.texcoord);
	}
	else
	{
		return float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
}