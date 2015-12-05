#pragma once
#include "ConstantBuffers.h"
class CCamera
{
private:
	XMFLOAT4X4 m_f44WorldMatrix;
public:
	CCamera();
	~CCamera();
	void CameraMovement();
	XMFLOAT4X4& GetWorldMatrix();
};