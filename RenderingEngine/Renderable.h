#pragma once
#include <d3d11.h>
class CRenderable
{
private:

public:
	CRenderable() = default;
	virtual ~CRenderable() = default;
	virtual void Process();
};