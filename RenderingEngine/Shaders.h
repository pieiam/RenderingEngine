#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>
class CShaders
{

private:
	std::unordered_map<std::string, ID3D11VertexShader*> m_mapVertexShaders;
	std::unordered_map<std::string, ID3D11PixelShader*> m_mapPixelShaders;
	std::unordered_map<std::string, ID3D11GeometryShader*> m_mapGeometryShaders;
	std::unordered_map<std::string, ID3D11HullShader*> m_mapHullShaders;
	std::unordered_map<std::string, ID3D11DomainShader*> m_mapDomainShaders;
	std::unordered_map<std::string, ID3D11ComputeShader*> m_mapComputeShaders;

	std::unordered_map<ID3D11VertexShader*, ID3D11InputLayout*> m_mapInputLayouts;
	// Helper Functions //
	void LoadShaders();
	void CreateInputLayouts();
public:
	CShaders();
	~CShaders();
	ID3D11VertexShader* GetVertexShader(std::string szShaderName);
	ID3D11PixelShader* GetPixelShader(std::string szShaderName);
	ID3D11GeometryShader* GetGeometryShader(std::string szShaderName);
	ID3D11HullShader* GetHullShader(std::string szShaderName);
	ID3D11DomainShader* GetDomainShader(std::string szShaderName);
	ID3D11ComputeShader* GetComputeShader(std::string szShaderName);
	ID3D11InputLayout* GetInputLayout(ID3D11VertexShader* vertShader);
};