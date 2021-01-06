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