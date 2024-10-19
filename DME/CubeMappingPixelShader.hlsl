#include "Common.hlsli"

TextureCube g_textureCube0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(CubeMappingPixelShaderInput input) : SV_TARGET
{
	return g_textureCube0.Sample(g_sampler, input.posModel.xyz);
}