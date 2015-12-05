#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT4;
//Union Declarations
#pragma region
union Position
{
	float m[4];
	struct
	{
		float x, y, z, w;
	};
};

union Normal
{
	float m[4];
	struct
	{
		float x, y, z, w;
	};
};
union TexCoords
{
	float m[4];
	struct
	{
		float x, y, z, w;
	};
};
union Tangent
{
	float m[4];
	struct
	{
		float x, y, z, w;
	};
};
#pragma endregion
struct TVertexDefault
{
	Position position;
	Normal normal;
	TexCoords texCoords;
};
struct TVertexUnlit
{
	Position position;
	TexCoords texCoords;
};
struct TVertexTangent
{
	Position position;
	Normal normal;
	TexCoords texCoords;
	Tangent tangent;
};