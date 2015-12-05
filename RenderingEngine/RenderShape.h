#pragma once
#include "RenderMesh3D.h"
enum ERenderShape{eFullScreenQuad,eRenderShapeTotal};
class CRenderShape : public CRenderMesh3D
{
private:
public:
	CRenderShape(ERenderShape eShape);
	~CRenderShape();
	void Process() override;
};