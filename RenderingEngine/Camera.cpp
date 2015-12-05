#include "Camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Time.h"
extern CTime gTime;
#define MOVE_SPEED 200.0f;

CCamera::CCamera()
{
	XMMATRIX matIdentity = XMMatrixIdentity();
	XMStoreFloat4x4(&m_f44WorldMatrix, matIdentity);
	m_f44WorldMatrix._42 = 50.0f;
	m_f44WorldMatrix._43 = -200.0f;
}
CCamera::~CCamera()
{

}
void CCamera::CameraMovement()
{
	float fDelta = (float)gTime.Delta();
	POINT curpoint;
	GetCursorPos(&curpoint);
	static POINT startpos = curpoint;

	XMMATRIX vmatrix = XMLoadFloat4x4(&m_f44WorldMatrix);
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		float changex = (float)(curpoint.x - startpos.x);
		float changey = (float)(curpoint.y - startpos.y);
		XMVECTOR pos = vmatrix.r[3];
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] = 0.0f;
		}
		vmatrix = XMMatrixMultiply(XMMatrixRotationX(changey*.01f), vmatrix);
		vmatrix = XMMatrixMultiply(vmatrix, XMMatrixRotationY(changex * .01f));
		for (size_t i = 0; i < 4; i++)
		{
			vmatrix.r[3].m128_f32[i] = pos.m128_f32[i];
		}
	}
	startpos = curpoint;

	if (GetAsyncKeyState('W'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] += vmatrix.r[2].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}
	else if (GetAsyncKeyState('S'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] -= vmatrix.r[2].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}

	if (GetAsyncKeyState('A'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] -= vmatrix.r[0].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}
	else if (GetAsyncKeyState('D'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] += vmatrix.r[0].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}

	if (GetAsyncKeyState('E'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] += vmatrix.r[1].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}
	else if (GetAsyncKeyState('Q'))
	{
		for (size_t i = 0; i < 3; i++)
		{
			vmatrix.r[3].m128_f32[i] -= vmatrix.r[1].m128_f32[i] * fDelta * MOVE_SPEED;
		}
	}
	XMStoreFloat4x4(&m_f44WorldMatrix, vmatrix);
}
XMFLOAT4X4& CCamera::GetWorldMatrix()
{
	return m_f44WorldMatrix;
}