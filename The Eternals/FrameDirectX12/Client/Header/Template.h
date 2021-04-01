#pragma once

const float MAX_RATIO = 1.0f;
const float MIN_RATIO = 0.0f;

template<class T1>
T1 LinearInterpolation(const T1& v1, const T1& v2, float& ratio)
{
	if (ratio >= MAX_RATIO)
		ratio = MAX_RATIO;

	else if (ratio <= MIN_RATIO)
		ratio = MIN_RATIO;

	return v1 * (1.0f - ratio) + v2 * ratio;
}