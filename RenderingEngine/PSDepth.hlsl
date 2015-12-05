#include "ShaderStructs.hlsli"
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	float4 baseColor = diffuseTexture.Sample(SimpleSampler, input.f4TexCoord.xy);
	if (baseColor.a < .1f)
		discard;
	return float4(0, 1, 0, 1);
}