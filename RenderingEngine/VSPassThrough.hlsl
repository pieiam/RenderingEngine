#include "ShaderStructs.hlsli"
PIXEL_DEFAULT main(VERTEX_DEFAULT input)
{
	PIXEL_DEFAULT output;
	output.f4Position = input.f4Position;
	output.f4WorldPos = output.f4Position;
	output.f4Normal = input.f4Normal;
	output.f4TexCoord = input.f4TexCoord;
	return output;
}