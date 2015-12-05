#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"
#define SPEC_POWER 2.0f
#define REFRACT_SCALE 0.02f
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	float4 baseColor = diffuseTexture.Sample(SimpleSampler, input.f4TexCoord.xy);
	if (baseColor.a < .1f)
		discard;
	float4 bump = normalTexture.Sample(SimpleSampler, input.f4TexCoord.xy) * 2.0f - 1.0f;

	float4 pointOutput = ComputePointLightsWithSpec(normalize(input.f4Normal.xyz), input.f4WorldPos.xyz, baseColor, input.f4Position, SPEC_POWER);
	float2 refractedUV = input.f4Position.xy;
	refractedUV.x /= WINDOW_WIDTH;
	refractedUV.y /= WINDOW_HEIGHT;
	refractedUV += bump.xy*REFRACT_SCALE;
	float4 refractedColor = colorTexture.Sample(SimpleSampler, refractedUV.xy);
	float4 Output = float4((pointOutput + (AMBIENTRATIO*baseColor) + refractedColor).xyz, baseColor.a);
	return Output;
}