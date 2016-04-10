#include "Lights.h"
#include "Renderer.h"
#include "ConstantBuffers.h"
#include "RenderMeshOBJ.h"
#include <fstream>

// Value between 2 input vals based on ratio
#define IN_BETWEEN_VAL(minV,maxV,r) {(((maxV) - (minV))*(r) + (minV))}
CLights::CLights()
{
	Initialize();
}
CLights::~CLights()
{

}
void CLights::Initialize()
{
	m_vtPointLights.resize(MAX_POINT_LIGHTS);
	m_vtPointLightStream.resize(MAX_POINT_LIGHTS);
	CRenderer& rend = CRenderer::GetInstance();
	for (unsigned int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		m_vpLightRenderables.push_back( new CRenderMeshOBJ("../RenderingEngine/Assets/Sphere.obj"));
		rend.AddRenderable(eContextWireframe, m_vpLightRenderables[i],false);
		CRenderMeshOBJ* pLightRender = static_cast<CRenderMeshOBJ*> (m_vpLightRenderables[i]);
		pLightRender->SetActive(false);
	}
	XMFLOAT4 f4MinPosRad = XMFLOAT4(-900.0f, 0, -900.0f, 50.0f);
	XMFLOAT4 f4MaxPosRad = XMFLOAT4(900.0f, 1000.0f, 900.0f, 500.0f);
	XMFLOAT4 f4MinColor = XMFLOAT4(0, 0, 0, 1);
	XMFLOAT4 f4MaxColor = XMFLOAT4(1, 1, 1, 1);
	//RandomizeLights(f4MinPosRad, f4MaxPosRad, f4MinColor, f4MaxColor);
	LoadLightsFromFile("LightScript.txt");
}
void CLights::UpdateLights()
{
	//HACK TODO remove
	if (GetAsyncKeyState('R') & 1)
	{
		//Unscale Debug
		for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
		{
			static_cast<CRenderMeshOBJ*>(m_vpLightRenderables[i])->UniformScale(1.0f/m_vtPointLightStream[i].f4PosRadius.w);
		}

		XMFLOAT4 f4MinPosRad = XMFLOAT4(-900.0f, 0, -900.0f, 50.0f);
		XMFLOAT4 f4MaxPosRad = XMFLOAT4(900.0f, 1000.0f, 900.0f, 500.0f);
		XMFLOAT4 f4MinColor = XMFLOAT4(0, 0, 0, 1);
		XMFLOAT4 f4MaxColor = XMFLOAT4(1, 1, 1, 1);
		RandomizeLights(f4MinPosRad, f4MaxPosRad, f4MinColor, f4MaxColor);
	}
	if (GetAsyncKeyState('L') & 1)
	{
		for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
		{
			CRenderMeshOBJ* pLightRender = static_cast<CRenderMeshOBJ*>(m_vpLightRenderables[i]);
			pLightRender->SetActive(!pLightRender->GetActive());
		}
	}
}
void CLights::RandomizeLights(XMFLOAT4 f4PosRadMin, XMFLOAT4 f4PosRadMax, XMFLOAT4 f4ColorMin, XMFLOAT4 f4ColorMax)
{
	// Forward declare Variables used in loop
	float fRatio, fX, fY, fZ,fRadius, fA,fR,fG,fB;
	for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		// Randomize ratio before each variable
		// Randomize Position
		fRatio = rand() / (float)RAND_MAX;
		fX = IN_BETWEEN_VAL(f4PosRadMin.x, f4PosRadMax.x, fRatio);
		fRatio = rand() / (float)RAND_MAX;
		fY = IN_BETWEEN_VAL(f4PosRadMin.y, f4PosRadMax.y, fRatio);
		fRatio = rand() / (float)RAND_MAX;
		fZ = IN_BETWEEN_VAL(f4PosRadMin.z, f4PosRadMax.z, fRatio);
		// Randomize Radius
		fRatio = rand() / (float)RAND_MAX;
		fRadius = IN_BETWEEN_VAL(f4PosRadMin.w, f4PosRadMax.w, fRatio);
		// Randomize Colors
		fRatio = rand() / (float)RAND_MAX;
		fR = IN_BETWEEN_VAL(f4ColorMin.x, f4ColorMax.x, fRatio);
		fRatio = rand() / (float)RAND_MAX;
		fG = IN_BETWEEN_VAL(f4ColorMin.y, f4ColorMax.y, fRatio);
		fRatio = rand() / (float)RAND_MAX;
		fB = IN_BETWEEN_VAL(f4ColorMin.z, f4ColorMax.z, fRatio);
		fRatio = rand() / (float)RAND_MAX;
		fA = IN_BETWEEN_VAL(f4ColorMin.w, f4ColorMax.w, fRatio);
		// Set Values on Point Light
		m_vtPointLights[i].f4PosRadius.x = m_vtPointLightStream[i].f4PosRadius.x = fX;
		m_vtPointLights[i].f4PosRadius.y = m_vtPointLightStream[i].f4PosRadius.y = fY;
		m_vtPointLights[i].f4PosRadius.z = m_vtPointLightStream[i].f4PosRadius.z = fZ;
		m_vtPointLights[i].f4PosRadius.w = m_vtPointLightStream[i].f4PosRadius.w = fRadius;
		m_vtPointLights[i].f4Color.x = fR;
		m_vtPointLights[i].f4Color.y = fG;
		m_vtPointLights[i].f4Color.z = fB;
		m_vtPointLights[i].f4Color.w = fA;
		CRenderMeshOBJ* pLightRender = static_cast<CRenderMeshOBJ*>( m_vpLightRenderables[i]);
		pLightRender->SetPosition(fX, fY, fZ);
		pLightRender->UniformScale(fRadius);
	}
	CRenderer& rend = CRenderer::GetInstance();
	rend.GetConstantBuffers()->MapToConstantBuffer(&m_vtPointLights[0], sizeof(TPointLight)*m_vtPointLights.size(), ePointLightBuffer);
	rend.GetConstantBuffers()->MapToConstantBuffer(&m_vtPointLightStream[0], sizeof(TPointLightStream)*m_vtPointLightStream.size(), ePointLightStreamBuffer);

}

//TODO this entire function
void CLights::LoadLightsFromFile(const char* _filename)
{
	//TODO 
	std::ifstream fin;
	fin.open(_filename);
	if (fin.is_open())
	{
		std::string line;
		unsigned int pointIndex = 0;
		while (std::getline(fin, line))
		{
			if (line.substr(0,5) == "Point")
			{
				// Read and Set Values on Point Light
				float f;
				fin >> f;
				m_vtPointLights[pointIndex].f4PosRadius.x = m_vtPointLightStream[pointIndex].f4PosRadius.x = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4PosRadius.y = m_vtPointLightStream[pointIndex].f4PosRadius.y = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4PosRadius.z = m_vtPointLightStream[pointIndex].f4PosRadius.z = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4PosRadius.w = m_vtPointLightStream[pointIndex].f4PosRadius.w = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4Color.x = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4Color.y = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4Color.z = f;
				fin >> f;
				m_vtPointLights[pointIndex].f4Color.w = f;

				// Update debug object
				CRenderMeshOBJ* pLightRender = static_cast<CRenderMeshOBJ*>(m_vpLightRenderables[pointIndex]);
				pLightRender->SetPosition(m_vtPointLights[pointIndex].f4PosRadius);
				pLightRender->UniformScale(m_vtPointLights[pointIndex].f4PosRadius.w);

				// Increment Point Light Counter
				++pointIndex;
			}
		}

		//Initialize the rest to zero
		for (unsigned int i = pointIndex; i < MAX_POINT_LIGHTS; ++i)
		{
			m_vtPointLights[i].f4PosRadius = XMFLOAT4(100, 100, 0, 0.0f);
			m_vtPointLights[i].f4Color = XMFLOAT4(1, 0, 0, 1);
			m_vtPointLightStream[i].f4PosRadius = XMFLOAT4(100, 100, 0, 0.0f);
		}

		//Map Constant Buffers
		CRenderer& rend = CRenderer::GetInstance();
		rend.GetConstantBuffers()->MapToConstantBuffer(&m_vtPointLights[0], sizeof(TPointLight)*m_vtPointLights.size(), ePointLightBuffer);
		rend.GetConstantBuffers()->MapToConstantBuffer(&m_vtPointLightStream[0], sizeof(TPointLightStream)*m_vtPointLightStream.size(), ePointLightStreamBuffer);

		fin.close();
	}

}