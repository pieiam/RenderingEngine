#ifndef LIGHTING_H_
#define LIGHTING_H_
#include "ShaderStructs.hlsli"
#define AMBIENTRATIO .2f
#define MAX_LIGHTS 1024
#define MAX_LIGHT_PER_TILE 512
#define SENTINEL_VALUE (MAX_LIGHTS+1)
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define TILE_SIZE 16

// 1 if using Phong, 0 if using Blinn-Phong
#define PHONG 0

#define SHININESS 512
struct POINTLIGHT
{
	float4 f4PosRadius;
	float4 f4Color;
};

cbuffer CPointLights : register (b2)
{
	POINTLIGHT PointLight[MAX_LIGHTS];
};
cbuffer CPointLightStream : register(b3)
{
	float4 PointLightPosRadius[MAX_LIGHTS];
}
float4 ComputePointLight(POINTLIGHT Light, float3 SurNormal, float3 SurPos,float4 SurColor )
{
	float3 f3LightDir = normalize(Light.f4PosRadius.xyz - SurPos);
	float LightRatio = saturate(dot(f3LightDir, normalize(SurNormal)));
	float Attenuation = 1.0 - saturate(length(Light.f4PosRadius.xyz - SurPos) / Light.f4PosRadius.w);
	return float4((Light.f4Color.xyz * LightRatio * SurColor.rgb * Attenuation * Attenuation), 0.0f);
}
float4 ComputePointLightWithSpec(POINTLIGHT Light, float3 SurNormal, float3 SurPos, float4 SurColor,float SpecModifier)
{
	float3 f3LightDir = normalize(Light.f4PosRadius.xyz - SurPos);
		float LightRatio = saturate(dot(f3LightDir, normalize(SurNormal)));
	float Attenuation = 1.0 - saturate(length(Light.f4PosRadius.xyz - SurPos) / Light.f4PosRadius.w);
	float4 f4Diffuse = float4((Light.f4Color.xyz * LightRatio * SurColor.rgb * Attenuation * Attenuation), 0.0f);
		float4 SpecColor = float4(0, 0, 0, 0);
		float3 ToCamera = normalize(f4CameraPos.xyz - Light.f4PosRadius.xyz);
#if PHONG
		//Phong Specular
		float3 ReflectVector = reflect(f3LightDir, SurNormal);
		float SpecIntensity = pow(clamp(dot(ToCamera, ReflectVector), 0, 1), SHININESS);
#else
		//Blinn-Phong Specular
		float3 HalfVector = normalize(f3LightDir + ToCamera);
		float SpecIntensity = pow(saturate(dot(SurNormal, HalfVector)), SHININESS);
#endif
	SpecColor = Light.f4Color * SpecIntensity * Attenuation * SpecModifier * SurColor;
	return f4Diffuse + SpecColor;
}
float4 ComputePointLights(float3 SurNormal, float3 SurPos, float4 SurColor, float4 PixelPosition)
{
	uint unTileID, unTileX, unTileY, unNumTilesRow;
	unNumTilesRow = (WINDOW_WIDTH / TILE_SIZE);
	unTileX = PixelPosition.x / TILE_SIZE;
	unTileY = PixelPosition.y / TILE_SIZE;
	unTileID = unTileX + unTileY * unNumTilesRow;
	uint unStart = unTileID * MAX_LIGHT_PER_TILE;
	float4 Output = float4(0, 0, 0, 0);
	for (unsigned int i = unStart;; i++)
	{
		if (LightIndexSRV[i] == SENTINEL_VALUE)
		{
			break;
		}
		Output += ComputePointLight(PointLight[LightIndexSRV[i]], SurNormal, SurPos, SurColor);
	}
	Output = saturate(Output);
	Output.a = SurColor.a;
	return Output;

}
float4 ComputePointLightsWithSpec(float3 SurNormal, float3 SurPos, float4 SurColor, float4 PixelPosition, float SpecModifier)
{
	uint unTileID, unTileX, unTileY, unNumTilesRow;
	unNumTilesRow = (WINDOW_WIDTH / TILE_SIZE);
	unTileX = PixelPosition.x / TILE_SIZE;
	unTileY = PixelPosition.y / TILE_SIZE;
	unTileID = unTileX + unTileY * unNumTilesRow;
	uint unStart = unTileID * MAX_LIGHT_PER_TILE;
	float4 Output = float4(0, 0, 0, 0);
		for (unsigned int i = unStart;; i++)
		{
			if (LightIndexSRV[i] == SENTINEL_VALUE)
			{
				break;
			}
			Output += ComputePointLightWithSpec(PointLight[LightIndexSRV[i]], SurNormal, SurPos, SurColor,SpecModifier);
		}
	Output = saturate(Output);
	Output.a = SurColor.a;
	return Output;

}
#endif