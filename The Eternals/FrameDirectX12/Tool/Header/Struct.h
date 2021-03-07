#pragma once


typedef struct tagFrame
{
	tagFrame()
		: fCurFrame(0.0f), fFrameCnt(1.0f)
		, fCurScene(0.0f), fSceneCnt(1.0f)
		, fFrameSpeed(1.0f)
	{}

	tagFrame(const _float& frame_cnt, const _float& scene_cnt, const _float& frame_speed)
		: fCurFrame(0.0f), fFrameCnt(frame_cnt)
		, fCurScene(0.0f), fSceneCnt(scene_cnt)
		, fFrameSpeed(frame_speed)
	{}

	_float	fFrameCnt	= 1.0f;	// 스프라이트 이미지의 X축 개수.
	_float	fCurFrame	= 0.0f;
	_float	fSceneCnt	= 1.0f;	// 스프라이트 이미지의 Y축 개수.
	_float	fCurScene	= 0.0f;

	_float	fFrameSpeed	= 0.0f;	// 스프라이트 이미지 재생속도.

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

typedef struct tagTextureTreeCtrlInfo
{
	wstring wstrTexType = L"";	// ex) Texture
	wstring wstrRootTag = L"";	// ex) UI
	wstring wstrTexTag	= L"";	// ex) S1UI_CharacterWindow

} TEX_TREECTRL_INFO;
