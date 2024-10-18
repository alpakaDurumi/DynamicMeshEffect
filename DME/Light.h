#pragma once

#include "DirectXMath.h"

using DirectX::XMFLOAT3;

struct Light {
	XMFLOAT3 strength = { 1.0f, 1.0f, 1.0f };
	float fallOffStart = 0.0f;
	XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f };
	float fallOffEnd = 10.0f;
	XMFLOAT3 position = { 0.0f, 0.0f, -5.0f };
	float spotPower = 10.0f;
};
