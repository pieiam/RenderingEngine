#include "Lighting.hlsli"
#include "LightCulling.hlsli"

#define FAR_PLANE 3000.0f
#define THREADS_PER_TILE 256

groupshared uint gsLightIndex[MAX_LIGHT_PER_TILE]; //Light index list
groupshared uint gsNumLights; //Current number of lights in list
groupshared uint gsMaxDepth; //Max Depth Value for the current Tile
groupshared uint gsMinDepth; //Min Depth Value for the current Tile
//16x16Tile Size
[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GroupID : SV_GroupID, uint3 ThreadID : SV_GroupThreadID)
{
	//Construct Frustum per tile

	//Screen Space Points on Far Plane
	float fLeft, fRight, fTop, fBottom; // Take Pixel Location divided by Total Pixels (in row/col) Then map from [0,1] to [-1,1] 
	fLeft = (TILE_SIZE * GroupID.x / (float)WINDOW_WIDTH)*2.0f - 1.0f;
	fTop = (TILE_SIZE * GroupID.y / (float)WINDOW_HEIGHT)*2.0f - 1.0f;
	fRight = (TILE_SIZE * (GroupID.x + 1) / (float)WINDOW_WIDTH)*2.0f - 1.0f;
	fBottom = (TILE_SIZE * (GroupID.y + 1) / (float)WINDOW_HEIGHT)*2.0f - 1.0f;

	fTop *= -1;
	fBottom *= -1;

	float3 f3FrustumPoints[4];
	f3FrustumPoints[0] = float3(fLeft, fTop, 1.0f); //TopLeft
	f3FrustumPoints[1] = float3(fRight, fTop, 1.0f); //TopRight
	f3FrustumPoints[2] = float3(fRight, fBottom, 1.0f); //BotRight
	f3FrustumPoints[3] = float3(fLeft, fBottom, 1.0f); //BotLeft
#if _RELEASE
	[unroll]
#endif
	for (unsigned int unCorner = 0; unCorner < 4; ++unCorner)
	{
		//Put frustum points into view space
		f3FrustumPoints[unCorner] = UnProjectPoint(float4(f3FrustumPoints[unCorner], 1.0f)).xyz;
	}
	TPlane tFrustumPlanes[4];
	tFrustumPlanes[0] = CreatePlane(f3FrustumPoints[0], f3FrustumPoints[1]);
	tFrustumPlanes[1] = CreatePlane(f3FrustumPoints[1], f3FrustumPoints[2]);
	tFrustumPlanes[2] = CreatePlane(f3FrustumPoints[2], f3FrustumPoints[3]);
	tFrustumPlanes[3] = CreatePlane(f3FrustumPoints[3], f3FrustumPoints[0]);
	//Only Reset Once
	if (ThreadID.x == 0 && ThreadID.y == 0)
	{
		gsNumLights = 0;
		gsMaxDepth = 0.0f;
		gsMinDepth = FAR_PLANE;
	}
	//Sync all threads in group
	GroupMemoryBarrierWithGroupSync();
	//Get Min and Max Depth from Depth Buffers
	//Sample Depth Buffer
	float fTempZ = depthTexture.Load(uint3(DTid.x, DTid.y, 0)).r;
	float fTempTranZ = transparencyTexture.Load(uint3(DTid.x, DTid.y, 0)).r;
	float fMinZ = min(fTempZ, fTempTranZ);
	//Unproject Depth Value
	fTempZ = UnProjectZ(fTempZ);
	fTempTranZ = UnProjectZ(fMinZ);
	//Convert To uint for groupshared
	uint uTempZ = asuint(fTempZ);
	uint uTempTranZ = asuint(fTempTranZ);
	InterlockedMax(gsMaxDepth, uTempZ);
	InterlockedMin(gsMinDepth, uTempTranZ);
	//Sync all threads in group
	GroupMemoryBarrierWithGroupSync();
	//Get Max and Min From GroupShared
	float zMax = asfloat(gsMaxDepth);
	float zMin = asfloat(gsMinDepth);
	//Loop through All Bound Lights
#if _RELEASE
	[unroll]
#endif
	for (unsigned int unLight = 0; unLight < MAX_POINT_LIGHTS; unLight += THREADS_PER_TILE)
	{
		uint index = unLight + (ThreadID.x + ThreadID.y * TILE_SIZE);
		//Get Light in View Space
		float4 curLight = PointLightPosRadius[index];
		float4 LightCenter = float4(curLight.xyz, 1);
		float Radius = curLight.w;

		//Check if Radius is above threshold value
		if (Radius > RADIUS_THRESHOLD)
		{

			LightCenter = mul(LightCenter, f44ViewMatrix);
			LightCenter /= LightCenter.w;
			//Check Depth
			if (LightCenter.z - zMax < Radius && -LightCenter.z + zMin < Radius)
			{
				//Check Light Against Frustum
				if (SphereToFrustum(tFrustumPlanes, LightCenter.xyz, Radius))
				{
					//Pushback index
					uint unBufferIndex = 0;
					InterlockedAdd(gsNumLights, 1, unBufferIndex);
					if (unBufferIndex < MAX_LIGHT_PER_TILE)
						gsLightIndex[unBufferIndex] = index;
				}
			}
		}

	}

	//Sync all threads in group
	GroupMemoryBarrierWithGroupSync();
	uint unNumTilesRow = (WINDOW_WIDTH / TILE_SIZE);
	uint unTileID = GroupID.x + GroupID.y * unNumTilesRow;
	uint unStart = unTileID * MAX_LIGHT_PER_TILE;
	//Transfer Data to UAV/SRV
#if _RELEASE
	[unroll]
#endif
	for (unsigned int BufferIndex = (ThreadID.x + ThreadID.y * TILE_SIZE); BufferIndex < gsNumLights; BufferIndex += THREADS_PER_TILE)
	{
		LightIndexUAV[unStart + BufferIndex] = gsLightIndex[BufferIndex];
	}
	//Only Mark End Once
	if (ThreadID.x == 0 && ThreadID.y == 0)
	{
		if (gsNumLights < MAX_LIGHT_PER_TILE)
			LightIndexUAV[unStart + gsNumLights] = SENTINEL_VALUE; // Marks End of list  Check against this while doing lighting calculations
		else
			LightIndexUAV[unStart + MAX_LIGHT_PER_TILE - 1] = SENTINEL_VALUE; // Marks End of list  Check against this while doing lighting calculations
	}

}