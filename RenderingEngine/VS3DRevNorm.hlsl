#include "ShaderStructs.hlsli"
PIXEL_DEFAULT main(VERTEX_DEFAULT input)
{
	PIXEL_DEFAULT output;
	output.f4Position = mul(input.f4Position, f44ObjectMatrix);
	output.f4WorldPos = output.f4Position;
	output.f4Position = mul(output.f4Position, f44ViewMatrix);
	output.f4Position = mul(output.f4Position, f44ProjMatrix);
	output.f4Normal = normalize(mul(float4(input.f4Normal.xyz * -1.0f, 0), f44ObjectMatrix));
	output.f4TexCoord = input.f4TexCoord;
	return output;
}