#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"
[earlydepthstencil]
float4 main(PIXEL_DEFAULT input) : SV_TARGET
{
	uint unTileID, unTileX, unTileY, unNumTilesRow;
	unNumTilesRow = (WINDOW_WIDTH / TILE_SIZE);
	unTileX =input.f4Position.x / TILE_SIZE;
	unTileY =input.f4Position.y / TILE_SIZE;
	unTileID = unTileX + unTileY * unNumTilesRow;
	uint unStart = unTileID * MAX_LIGHT_PER_TILE;
	uint unNumLights = 0;
	for (unsigned int i = unStart;; ++i)
	{
		if (LightIndexSRV[i] == SENTINEL_VALUE)
		{
			break;
		}
		++unNumLights;
	}
	float fColorVal = unNumLights / (float)MAX_LIGHT_PER_TILE; 
	float4 Output = float4(fColorVal, fColorVal, fColorVal, .5f);
	return Output;
}