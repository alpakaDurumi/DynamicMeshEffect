#include "Common.hlsli"

struct ShellVertexShaderInput
{
	float3 posModel : POSITION;
	float3 normalModel : NORMAL0;
	float2 texcoord : TEXCOORD0;
	float3 faceNormal : NORMAL1;
	uint isOutside : TEXCOORD1;
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
	
	float3 faceNormal = normalize(mul(float4(input.faceNormal, 0.0f), invTranspose)).xyz;
	
	// ���� ���� �ִٸ� ��ġ ��ȭ
	float d = distance(mousePos, pos.xyz);
	if (d <= radius)
	{
		// �Ÿ��� �������� ȿ���� Ŀ����
		float intensity = (radius - d) / radius;
		float3 displacement = faceNormal * intensity * 0.1f;
		
		//if (input.isOutside == 1)
		//{
		//	pos = float4(pos.xyz + displacement, 1.0f);
		//}
		//else
		//{
		//	pos = float4(pos.xyz - displacement, 1.0f);
		//}
		pos = float4(pos.xyz + displacement, 1.0f);
	}
	
	output.posWorld = pos.xyz;
	
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.posProj = pos;
	output.texcoord = input.texcoord;
	
	return output;
}