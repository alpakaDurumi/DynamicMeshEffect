cbuffer VertexConstantData : register(b0)
{
	matrix viewProj;
};

struct VertexShaderInput
{
	float3 posModel : POSITION;
	float3 normalModel : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct CubeMappingPixelShaderInput
{
	float4 posProj : SV_POSITION;
	float3 posModel : POSITION;
};

CubeMappingPixelShaderInput main(VertexShaderInput input)
{
	CubeMappingPixelShaderInput output;
	output.posModel = input.posModel;
	output.posProj = mul(float4(input.posModel, 1.0f), viewProj);
	
	return output;
}