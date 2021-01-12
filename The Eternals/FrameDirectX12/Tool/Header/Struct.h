#pragma once

typedef struct tagFrame
{
	tagFrame()
		: fFrameOffset(0.0f), iFrameCnt(1)
		, fSceneOffset(0.0f), iSceneCnt(1)
		, fFrameSpeed(1.0f)
	{}

	tagFrame(const _int& frame_cnt, const _int& scene_cnt, const _float& frame_speed)
		: fFrameOffset(0.0f), iFrameCnt(frame_cnt)
		, fSceneOffset(0.0f), iSceneCnt(scene_cnt)
		, fFrameSpeed(frame_speed)
	{}

	_int	iFrameCnt		= 1;	// 스프라이트 이미지의 X축 개수.
	_float	fFrameOffset	= 0.0f;
	_int	iSceneCnt		= 1;	// 스프라이트 이미지의 Y축 개수.
	_float	fSceneOffset	= 0.0f;

	_float	fFrameSpeed		= 0.0f;	// 스프라이트 이미지 재생속도.

} FRAME;

typedef struct tagTexturePath
{
	wstring wstrTextureTag	= L"";	// TextureTag.
	wstring wstrPath		= L"";	// 파일 상대 경로.
	_int	iTexSize		= 1;	// Texture 개수.

} IMGPATH;

typedef struct tagMeshPath
{
	wstring wstrMeshTag		= L"";	// MeshTag.
	wstring wstrPath		= L"";	// 파일 상대 경로.
	wstring wstrFileName	= L"";	// Mesh 파일 이름.

} MESHPATH;

typedef struct tagMeshTreeCtrlInfo
{
	wstring wstrMeshType	= L"";	// ex) StaticMesh
	wstring wstrRootTag		= L"";	// ex) Stage1
	wstring wstrMeshTag		= L"";	// ex) LandMark

} MESH_TREECTRL_INFO;