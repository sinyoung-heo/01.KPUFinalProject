#include "stdafx.h"
#include "ToolFileInfo.h"

CString CToolFileInfo::ConvertRelativePath(CString strFullPath)
{
	// 1. ����θ� ������ ���ڿ�.
	_tchar szRelativePath[MAX_STR] = L"";

	// 2. ���� �۾� ��θ� ������ ���ڿ�.
	_tchar szCurrentPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szCurrentPath);

	// 3. ����� ���ϱ�.
	PathRelativePathTo(szRelativePath,				// ��� ��θ� �޴� ���ڿ��� ���� ������.
					   szCurrentPath,				// ��� ����� ������ �����ϴ� ��θ� �����ϴ� ������.
					   FILE_ATTRIBUTE_DIRECTORY,	// ������ �Ӽ�.
					   strFullPath,					// ��� ����� ������ �����ϴ� ��θ� �����ϴ� ������.
					   FILE_ATTRIBUTE_DIRECTORY);	// ������ �Ӽ�.

	return szRelativePath;
}

void CToolFileInfo::DirInfoExtractionDDS(const wstring& wstrPath, list<IMGPATH*>& rPathInfoLst)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// ���� �� ��� ���� ���� ����. (MFC Ŭ����)
	CFileFind find;

	// ���� ��ο��� ù ��° ���� Ž��.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFileW();

		// ���� ã�� ���� �̸��� "." �̳� ".."���� �˻�. ������ true ��ȯ.
		if (find.IsDots())
			continue;

		// ã�� �̸��� ���ϸ��� �ƴ� �������̶�� true��ȯ.
		else if (find.IsDirectory())
			DirInfoExtractionDDS(wstring(find.GetFilePath()), rPathInfoLst);

		// find ��ü�� ã�� ���� ���� �̸��� ��.
		else
		{
			if (find.IsSystem())
				continue;

			IMGPATH* pImgPath = new IMGPATH;
			_tchar szCurPath[MAX_STR] = L"";

			/*__________________________________________________________________________________________________________
			[ ���� ��� ���� ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire\0.dds
			____________________________________________________________________________________________________________*/
			lstrcpy(szCurPath, find.GetFilePath().GetString());

			/*__________________________________________________________________________________________________________
			[ ���ϸ��� Ȯ���� ���� ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire
			____________________________________________________________________________________________________________*/
			PathRemoveFileSpec(szCurPath);

			/*__________________________________________________________________________________________________________
			[ ���� ���� ���� ������ ������ ��´�. ]
			____________________________________________________________________________________________________________*/
			pImgPath->iTexSize = DirFileCount(szCurPath);

			/*__________________________________________________________________________________________________________
			[ ��� �󿡼� ������ ���ϸ��� ���´�. ]
			ex) wstrTexturename = 0, 1, 2, 3, 4 ....
			____________________________________________________________________________________________________________*/
			wstring wstrTextureName = find.GetFileTitle().GetString();

			// wstrTextureName = 0.dds
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.length() - 1) + L"%d.dds";

			// szFullPath : D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire\%d.dds
			_tchar szFullPath[MAX_STR] = L"";
			PathCombine(szFullPath, szCurPath, wstrTextureName.c_str());

			/*__________________________________________________________________________________________________________
			[ ��� ��� ��ȯ. ]
			____________________________________________________________________________________________________________*/
			pImgPath->wstrPath			= ConvertRelativePath(szFullPath);
			pImgPath->wstrTextureTag	= PathFindFileName(szCurPath);

			rPathInfoLst.push_back(pImgPath);

			iContinue = 0;
		}
	}
}

void CToolFileInfo::DirInfoMeshExtraction(const wstring& wstrPath, list<MESHPATH*>& rPathInfoLst)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// ���� �� ��� ���� ���� ����. (MFC Ŭ����)
	CFileFind find;

	// ���� ��ο��� ù ��° ���� Ž��.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFileW();

		// ���� ã�� ���� �̸��� "." �̳� ".."���� �˻�. ������ true ��ȯ.
		if (find.IsDots())
			continue;

		// ã�� �̸��� ���ϸ��� �ƴ� �������̶�� true��ȯ.
		else if (find.IsDirectory())
			DirInfoMeshExtraction(wstring(find.GetFilePath()), rPathInfoLst);

		// find ��ü�� ã�� ���� ���� �̸��� ��.
		else
		{
			if (find.IsSystem())
				continue;

			MESHPATH*	pMeshPath = new MESHPATH;
			_tchar		szCurPath[MAX_STR] = L"";

			/*__________________________________________________________________________________________________________
			[ ���� ��� ���� ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\StaticMesh\LandMark\HD_LandMark_Diff.dds
			____________________________________________________________________________________________________________*/
			lstrcpy(szCurPath, find.GetFilePath().GetString());

			/*__________________________________________________________________________________________________________
			[ ���ϸ��� Ȯ���� ���� ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\StaticMesh\LandMark\
			____________________________________________________________________________________________________________*/
			PathRemoveFileSpec(szCurPath);

			/*__________________________________________________________________________________________________________
			[ �������� ���ͼ� MeshTag�� MeshFileName ������ ]
			____________________________________________________________________________________________________________*/
			CFileFind find_directory;
			find_directory.FindFile(szCurPath);

			_int iResult = 1;
			while (iResult)
			{
				iResult = find_directory.FindNextFileW();

				// ���� ã�� ���� �̸��� "." �̳� ".."���� �˻�. ������ true ��ȯ.
				if (find_directory.IsDots())
					continue;

				// ã�� �̸��� ���ϸ��� �ƴ� �������̶�� true ��ȯ.
				if (find_directory.IsDirectory())
				{
					iResult = 0;

					pMeshPath->wstrMeshTag	= find_directory.GetFileName();
					pMeshPath->wstrFileName = pMeshPath->wstrMeshTag + L".X";
				}
			}

			/*__________________________________________________________________________________________________________
			[ ��� ��� ��ȯ. ]
			____________________________________________________________________________________________________________*/
			pMeshPath->wstrPath = ConvertRelativePath(szCurPath);
			pMeshPath->wstrPath = pMeshPath->wstrPath + L"\\";
			rPathInfoLst.push_back(pMeshPath);

			iContinue = 0;
		}
	}
}

_int CToolFileInfo::DirFileCount(const wstring& wstrPath)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	CFileFind find;
	find.FindFile(wstrFilePath.c_str());

	_int iFileCnt = 0;
	_int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFile();

		if (find.IsDots())
			continue;
		else if (find.IsDirectory())
			continue;
		else if (find.IsSystem())
			continue;

		++iFileCnt;
	}

	return iFileCnt;
}