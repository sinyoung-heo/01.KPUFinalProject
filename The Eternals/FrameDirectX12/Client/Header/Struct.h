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