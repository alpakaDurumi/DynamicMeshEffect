#include "Common.hlsli"

cbuffer VertexConstantData : register(b0)
{
	matrix viewProj;
};

CubeMappingPixelShaderInput main(VertexShaderInput input)
{
	CubeMappingPixelShaderInput output;
	output.posModel = input.posModel;
	output.posProj = mul(float4(input.posModel, 1.0f), viewProj);
	
	return output;
}