#pragma once
#include "stdafx.h"


inline float returnNoise2D(float x, float y)
{
	int n = (int)x + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((float)nn / 1073741824.0f);
}

inline float perlinNoise1DForSin(float x)
{
	return sin(2 * x) + sin(3.141592f * x);
}

inline float myInterpolation(float a, float b, float t) {
	float step = 6 * pow(t, 5) - 15 * pow(t, 4) * 10 * pow(t, 3);
	return a + (b - a) * step;
}

inline float SmoothStep(float t) {
	return t * t * (3.f - 2.f * t);
}

inline float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

inline float GetPerlinNoise(float x, float y)
{
	float result;

	XMVECTOR gradient[4];
	XMVECTOR vertices[4];
	XMVECTOR distanceVec[4];
	XMVECTOR influence[4];

	XMVECTOR pos{ x,y };

	vertices[0] = XMVECTOR({ static_cast<float>((int)x), static_cast<float>((int)y) });
	vertices[1] = XMVECTOR({ static_cast<float>((int)x + 1), static_cast<float>((int)y) });
	vertices[2] = XMVECTOR({ static_cast<float>((int)x), static_cast<float>((int)y + 1) });
	vertices[3] = XMVECTOR({ static_cast<float>((int)x + 1), static_cast<float>((int)y + 1) });

	for (int i = 0; i < 4; ++i) {
		//gradient[i] = XMVECTOR({ perlinNoise1DForSin(vertices[i].m128_f32[0]), perlinNoise1DForSin(vertices[i].m128_f32[1]) });
		/*XMFLOAT2 vec = RandomFloatInRangeSeed(vertices[i].m128_f32[0], vertices[i].m128_f32[1]);
		gradient[i] = XMVECTOR({ vec.x, vec.y });*/
		gradient[i] = XMVECTOR({ RandomFloatInRange(-1.0f, 1.0f), RandomFloatInRange(-1.0f, 1.0f) });
		//gradient[i] = XMVector2Normalize(gradient[i]);
		distanceVec[i] = pos - vertices[i];
		influence[i] = XMVector2Dot(XMVector2Normalize(gradient[i]), distanceVec[i]);
	}

	/*XMVECTOR lerp1 = XMVectorLerp(influence[0], influence[1], x - (int)x);
	XMVECTOR lerp2 = XMVectorLerp(influence[2], influence[3], x - (int)x);

	result = XMVectorLerp(lerp1, lerp2, y - (int)y).m128_f32[0];*/


	/*float flerp1 = myInterpolation(influence[0].m128_f32[0], influence[1].m128_f32[0], x - (int)x);
	float flerp2 = myInterpolation(influence[2].m128_f32[0], influence[3].m128_f32[0], x - (int)x);

	result = myInterpolation(flerp1, flerp2, y - (int)y);*/

	float flerp1 = Lerp(influence[0].m128_f32[0], influence[1].m128_f32[0], x - (int)x);
	float flerp2 = Lerp(influence[2].m128_f32[0], influence[3].m128_f32[0], x - (int)x);

	result = Lerp(flerp1, flerp2, y - (int)y);

	return result;
}

inline void GeneratePerlinNoise(int width, int height, float scale, float octaves, float persistance, float lacunarity, std::vector<XMFLOAT4>& returnData) {
	returnData.resize(width * height);
	scale = max(0.0001f, scale);

	float maxNoiseHeight = FLT_MIN; //최대 값을 담기 위한 변수
	float minNoiseHeight = FLT_MAX; //최소 값을 담기 위한 변수

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float amplitude = 1;
			float frequency = 0.1;
			float noiseHeight = 0;

			for (int i = 0; i < octaves; ++i) {
				float perlinValue = GetPerlinNoise((float)x / scale * frequency, (float) y / scale * frequency);
				noiseHeight += perlinValue * amplitude;
				amplitude *= persistance;
				frequency *= lacunarity;
			}
			if (noiseHeight > maxNoiseHeight) maxNoiseHeight = noiseHeight;
			else if (noiseHeight < minNoiseHeight) minNoiseHeight = noiseHeight;
			returnData[y * width + x] = XMFLOAT4(noiseHeight, noiseHeight, noiseHeight, noiseHeight);
		}
	}
	float valueArea =maxNoiseHeight - minNoiseHeight;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			float newValue = (returnData[y * width + x].x - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
			//float newValue = (returnData[y * width + x].x - minNoiseHeight) / valueArea;
			returnData[y * width + x] = XMFLOAT4(newValue, newValue, newValue, newValue);
		}
	}
}