#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"
#define REFRACT_SCALE 0.05f
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{

	float2 refractedUV = input.f4Position.xy;
	refractedUV.x /= WINDOW_WIDTH;
	refractedUV.y /= WINDOW_HEIGHT;
	refractedUV.y -= REFRACT_SCALE;
	float4 refractedColor = colorTexture.Sample(SimpleSampler, refractedUV.xy);
		refractedColor += float4(.2, .2, .4f, 0);
	float4 Output = float4(refractedColor.xyz, .50f);
	return Output;
}