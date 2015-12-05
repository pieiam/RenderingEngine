#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"
[earlydepthstencil]
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	float4 baseColor = diffuseTexture.Sample(SimpleSampler, input.f4TexCoord.xy);
	if (baseColor.a < .1f)
		discard;
	float4 pointOutput = ComputePointLights(normalize(input.f4Normal.xyz), input.f4WorldPos.xyz, baseColor, input.f4Position);


	return pointOutput;
}