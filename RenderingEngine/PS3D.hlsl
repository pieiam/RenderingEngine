#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"
#define SPEC_POWER .05f
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	float4 baseColor = diffuseTexture.Sample(SimpleSampler, input.f4TexCoord.xy);
	if (baseColor.a < .1f)
		discard;

	float4 pointOutput = ComputePointLightsWithSpec(normalize(input.f4Normal.xyz), input.f4WorldPos.xyz, baseColor, input.f4Position,SPEC_POWER);

	return pointOutput + (AMBIENTRATIO*baseColor);
}