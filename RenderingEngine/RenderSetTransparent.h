#pragma once
#include "RenderSet.h"
class CRenderSetTransparent :public CRenderSet
{
private:
	CRenderContext* m_pcRenderContextFrontCulled = nullptr;
public:
	void Process() override;
	virtual void SetRenderContext(CRenderContext* pcRenderContext);
	void DepthProcess() override;
	CRenderSetTransparent();
	~CRenderSetTransparent();
};