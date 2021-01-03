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

	_int	iFrameCnt		= 1;	// ��������Ʈ �̹����� X�� ����.
	_float	fFrameOffset	= 0.0f;
	_int	iSceneCnt		= 1;	// ��������Ʈ �̹����� Y�� ����.
	_float	fSceneOffset	= 0.0f;

	_float	fFrameSpeed		= 0.0f;	// ��������Ʈ �̹��� ����ӵ�.

} FRAME;