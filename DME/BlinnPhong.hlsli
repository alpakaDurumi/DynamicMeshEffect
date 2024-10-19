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
	// halfway vector ���
	float3 halfway = normalize(viewDir + lightVec);
	
	// specular ���
	float hdotn = max(dot(halfway, normal), 0.0f);
	float3 specular = mat.specular * pow(hdotn, mat.shininess);

	return mat.ambient + mat.diffuse * lightStrength + specular;
}

float3 ComputeDirectionalLight(Light light, Material mat, float3 normal, float3 viewDir)
{	
	// ǥ�鿡�� ���������� ����
	float3 lightDir = -light.direction;

	// ������ ���� ���(Lambertian reflectance)
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
	// ǥ�鿡�� ���������� ����	
	float3 lightDir = light.position - position;

    // ǥ�鿡������ ���������� �Ÿ�
	float d = length(lightDir);

	// �Ÿ��� ���� ���� ����
	if (d > light.fallOffEnd)
	{
		return mat.ambient;
	}
	else
	{
		// normalization
		lightDir /= d;

		// ������ ���� ���(Lambertian reflectance)
		float ndotl = max(dot(lightDir, normal), 0.0f);
		float3 lightStrength = light.strength * ndotl;

		// ���� ���
		float att = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
		lightStrength *= att;

		return BlinnPhong(lightStrength, lightDir, mat, normal, viewDir);
	}
}

float3 ComputeSpotLight(Light light, Material mat, float3 position, float3 normal, float3 viewDir)
{
	// ǥ�鿡�� ���������� ����	
	float3 lightDir = light.position - position;

    // ǥ�鿡������ ���������� �Ÿ�
	float d = length(lightDir);

    // �Ÿ��� ���� ���� ����
	if (d > light.fallOffEnd)
	{
		return mat.ambient;
	}
	else
	{
		// normalization
		lightDir /= d;

		// ������ ���� ���(Lambertian reflectance)
		float ndotl = max(dot(lightDir, normal), 0.0f);
		float3 lightStrength = light.strength * ndotl;

		// ���� ���
		float att = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
		lightStrength *= att;

		// ������ ����Ű�� ����� �������� ǥ�������� ������ �����Ҽ��� ���� ����
		float spotFactor = pow(max(dot(-lightDir, light.direction), 0.0f), light.spotPower);
		lightStrength *= spotFactor;

		return BlinnPhong(lightStrength, lightDir, mat, normal, viewDir);
	}
}

#endif // __BLINNPHONG_HLSLI__