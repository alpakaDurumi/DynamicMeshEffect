#ifndef __BLINNPHONG_HLSLI__
#define __BLINNPHONG_HLSLI__

struct Material
{
	float3 ambient;
	float shininess;
	float3 diffuse;
	float padding1;
	float3 specular;
	float padding2;
};

struct Light
{
	float3 strength;
	float fallOffStart;
	float3 direction;
	float fallOffEnd;
	float3 position;
	float spotPower;
};

float3 BlinnPhong(float3 lightStrength, float3 lightVec, Material mat, float3 normal, float3 viewDir)
{
	// halfway vector 계산
	float3 halfway = normalize(viewDir + lightVec);
	
	// specular 계산
	float hdotn = max(dot(halfway, normal), 0.0f);
	float3 specular = mat.specular * pow(hdotn, mat.shininess);

	return mat.ambient + mat.diffuse * lightStrength + specular;
}

float3 ComputeDirectionalLight(Light light, Material mat, float3 normal, float3 viewDir)
{	
	// 표면에서 광원으로의 방향
	float3 lightDir = -light.direction;

	// 광원의 세기 계산(Lambertian reflectance)
	float ndotl = max(dot(lightDir, normal), 0.0f);
	float3 lightStrength = light.strength * ndotl;

	return BlinnPhong(lightStrength, lightDir, mat, normal, viewDir);
}

float CalcAttenuation(float distance, float falloffStart, float falloffEnd)
{
    // Linear falloff
	return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(Light light, Material mat, float3 position, float3 normal, float3 viewDir)
{
	// 표면에서 광원으로의 방향	
	float3 lightDir = light.position - position;

    // 표면에서부터 광원까지의 거리
	float d = length(lightDir);

	// 거리에 따라 조명 적용
	if (d > light.fallOffEnd)
	{
		return mat.ambient;
	}
	else
	{
		// normalization
		lightDir /= d;

		// 광원의 세기 계산(Lambertian reflectance)
		float ndotl = max(dot(lightDir, normal), 0.0f);
		float3 lightStrength = light.strength * ndotl;

		// 감쇠 계산
		float att = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
		lightStrength *= att;

		return BlinnPhong(lightStrength, lightDir, mat, normal, viewDir);
	}
}

float3 ComputeSpotLight(Light light, Material mat, float3 position, float3 normal, float3 viewDir)
{
	// 표면에서 광원으로의 방향	
	float3 lightDir = light.position - position;

    // 표면에서부터 광원까지의 거리
	float d = length(lightDir);

    // 거리에 따라 조명 적용
	if (d > light.fallOffEnd)
	{
		return mat.ambient;
	}
	else
	{
		// normalization
		lightDir /= d;

		// 광원의 세기 계산(Lambertian reflectance)
		float ndotl = max(dot(lightDir, normal), 0.0f);
		float3 lightStrength = light.strength * ndotl;

		// 감쇠 계산
		float att = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
		lightStrength *= att;

		// 광원이 가리키는 방향과 광원에서 표면으로의 방향이 유사할수록 세기 증가
		float spotFactor = pow(max(dot(-lightDir, light.direction), 0.0f), light.spotPower);
		lightStrength *= spotFactor;

		return BlinnPhong(lightStrength, lightDir, mat, normal, viewDir);
	}
}

#endif // __BLINNPHONG_HLSLI__