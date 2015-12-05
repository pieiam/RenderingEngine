#pragma once
#include <unordered_map>
#include <string>

#include "DDSTextureLoader.h"

class CTextureManager
{
private:
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_mpShaderResourceViews;

	//Credit To Conner McLaine
	void FindFilesByPath(std::string folderPath, std::string fileExtension, std::vector<std::string>& filePaths);

public:
	CTextureManager();
	~CTextureManager();
	ID3D11ShaderResourceView* GetSRV(std::wstring _TexPath){ return m_mpShaderResourceViews[_TexPath]; }
};