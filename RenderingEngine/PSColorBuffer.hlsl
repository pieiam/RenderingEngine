#include "ShaderStructs.hlsli"

float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	//Render Color From Color Buffer TODO
	float4 Output = colorTexture.Sample(SimpleSampler, input.f4TexCoord.xy);
	return Output;
}