#include "Common.hlsli"

struct ShellVertexShaderInput
{
	float3 posModel : POSITION;
	float3 normalModel : NORMAL;
	float2 texcoord : TEXCOORD0;
	bool isOutside : TEXCOORD1;
};

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

PixelShaderInput main(ShellVertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.posModel, 1.0f);
	pos = mul(pos, model);
	
	// ��� ���� ��ȯ ���� ����
	float4 normal = float4(input.normalModel, 0.0f);
	output.normalWorld = mul(normal, invTranspose).xyz;
	output.normalWorld = normalize(output.normalWorld);
	
	// ���� ���� �ִٸ� ��ġ ��ȭ
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