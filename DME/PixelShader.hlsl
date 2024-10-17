Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

struct PixelShaderInput
{
	float4 posProj : SV_POSITION; // projection space
	float3 posWorld : POSITION; // world space
	float3 normalWorld : NORMAL;
	float2 texcoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}