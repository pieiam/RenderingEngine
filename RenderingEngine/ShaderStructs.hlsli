#ifndef SHADER_STRUCTS_H_
#define SHADER_STRUCTS_H_

#pragma pack_matrix(row_major)
struct VERTEX_DEFAULT
{
	float4 f4Position : POSITION;
	float4 f4Normal : NORMAL;
	float4 f4TexCoord : TEXCOORD;
};
struct PIXEL_DEFAULT
{
	float4 f4Position : SV_POSITION;
	float4 f4Normal : NORMAL;
	float4 f4TexCoord : TEXCOORD;
	float4 f4WorldPos : POSITION;
};
cbuffer CObject : register (b0)
{
	float4x4 f44ObjectMatrix;
};
cbuffer CScene : register (b1)
{
	float4x4 f44ViewMatrix;
	float4x4 f44ProjMatrix;
	float4x4 f44InvProj;
	float4 f4CameraPos;
};
// NOTE: Lighting Constant Buffers are Declared in "Lighting.hlsli"
SamplerState SimpleSampler : register(s0);

texture2D diffuseTexture : register(t0);
texture2D normalTexture : register(t1);
texture2D specularTexture : register(t2);
Buffer<uint> LightIndexSRV : register(t3);
texture2D depthTexture : register(t4); // Depth Buffer for opaque objects
texture2D transparencyTexture : register(t5); // Depth Buffer for transparent objects
texture2D colorTexture : register(t6); // Color Buffer for refraction

RWBuffer<uint> LightIndexUAV : register(u0);
#endif