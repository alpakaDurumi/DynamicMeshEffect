#include "Common.hlsli"

cbuffer VertexConstantData : register(b0)
{
	matrix model;
	matrix invTranspose;
	matrix view;
	matrix projection;
};

cbuffer ShellVertexConstantData : register(b1)
{
	float3 mousePos;
	float radius;
}

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.posModel, 1.0f);
	pos = mul(pos, model);
	
	// 노멀 벡터 변환 먼저 수행
	float4 normal = float4(input.normalModel, 0.0f);
	output.normalWorld = mul(normal, invTranspose).xyz;
	output.normalWorld = normalize(output.normalWorld);
	
	// 범위 내에 있다면 위치 변화
	float d = distance(mousePos, pos.xyz);
	if (d <= radius)
	{
		pos = float4(pos.xyz + output.normalWorld * d * 0.1f, 1.0f);
	}
	
	output.posWorld = pos.xyz;
	
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.posProj = pos;
	output.texcoord = input.texcoord;
	
	return output;
}