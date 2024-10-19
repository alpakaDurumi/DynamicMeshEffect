struct VertexShaderInput
{
	float3 posModel : POSITION;
	float3 normalModel : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct PixelShaderInput
{
	float4 posProj : SV_POSITION; // projection space
	float3 posWorld : POSITION; // world space
	float3 normalWorld : NORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer VertexConstantData : register(b0)
{
	matrix model;
	matrix invTranspose;
	matrix view;
	matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.posModel, 1.0f);
	pos = mul(pos, model);
    
	output.posWorld = pos.xyz;

	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.posProj = pos;
	output.texcoord = input.texcoord;
    
	float4 normal = float4(input.normalModel, 0.0f);
	output.normalWorld = mul(normal, invTranspose).xyz;
	output.normalWorld = normalize(output.normalWorld);
	
	return output;
}