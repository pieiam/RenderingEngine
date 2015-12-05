#ifndef LIGHT_CULLING_H_
#define LIGHT_CULLING_H_
#include "ShaderStructs.hlsli"
//View Space Plane
struct TPlane
{
	float3 f3Normal;
};
//Take Screen Space Points into View Space
float4 UnProjectPoint(float4 f4ScreenSpace)
{
	f4ScreenSpace = mul(f4ScreenSpace, f44InvProj);
	f4ScreenSpace /= f4ScreenSpace.w;
	return f4ScreenSpace;
}
//Create View Space Plane
TPlane CreatePlane(float3 f3P1, float3 f3P2)
{
	//Only need 2 points because 3rd is Camera Pos(origin in View Space)
	TPlane Output;
	Output.f3Normal = normalize(cross(f3P1, f3P2));
	return Output;
}
bool SphereToFrustum(TPlane FrustumPlane[4], float3 Center, float Radius)
{
	if (dot(FrustumPlane[1].f3Normal, Center) < Radius
		&& dot(FrustumPlane[3].f3Normal, Center) < Radius
		&& dot(FrustumPlane[0].f3Normal, Center) < Radius
		&& dot(FrustumPlane[2].f3Normal, Center) < Radius)
	{
		return true;
	}
	return false;
}
//Take Post Projection Depth and Convert to View Space Depth
float UnProjectZ(float z)
{
	// Take Inverse of Z multiplied by Z scaling added with Overall Scaling factor
	z = 1.0f / (z*f44InvProj._34 + f44InvProj._44);
	return z;
}
#endif