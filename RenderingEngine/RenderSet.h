#pragma once
#include "Renderable.h"
#include <vector>
class CRenderContext;
class CRenderSet : public CRenderable
{
protected:
	CRenderContext* m_pcRenderContext;
	CRenderContext* m_pcRenderContextDepth = nullptr;
	std::vector<CRenderable*> m_vpRenderables;
public:
	CRenderSet();
	virtual ~CRenderSet();
	virtual void Process() override;
	virtual void DepthProcess();
	// Accessors and Mutators //
	virtual void SetRenderContext(CRenderContext* pcRenderContext);
	CRenderContext* GetRenderContext();

	std::vector<CRenderable*>& GetRenderables();
};