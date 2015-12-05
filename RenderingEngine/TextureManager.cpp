#include "TextureManager.h"
#include <vector>
#include <Windows.h>
#include "Renderer.h"
CTextureManager::CTextureManager()
{
	CRenderer& rend = CRenderer::GetInstance();
	std::vector<std::string> texturePaths;
	FindFilesByPath("../RenderingEngine/Assets/", ".dds", texturePaths);

	for (size_t currTex = 0; currTex < texturePaths.size(); currTex++)
	{
		std::wstring wPath(texturePaths[currTex].begin(), texturePaths[currTex].end());
		HRESULT hr = CreateDDSTextureFromFile(rend.GetDevice() ,wPath.c_str(), NULL, &m_mpShaderResourceViews[wPath]);

	}

}
CTextureManager::~CTextureManager()
{
	auto iter = m_mpShaderResourceViews.begin();
	while (iter != m_mpShaderResourceViews.end())
	{
		SAFE_RELEASE(iter->second);
		++iter;
	}
}

void CTextureManager::FindFilesByPath(std::string folderPath, std::string fileExtension, std::vector<std::string>& filePaths)
{
	std::string search_path = folderPath + "*" + fileExtension;
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string path = folderPath + fd.cFileName;
				filePaths.push_back(path);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
}