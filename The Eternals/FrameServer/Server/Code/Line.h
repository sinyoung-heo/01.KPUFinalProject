#pragma once

class CLine
{
public:
	enum POINT { POINT_START, POINT_FINISH, POINT_END };
	enum COMPARE { COMPARE_LEFT, COMPARE_RIGHT };

private:
	explicit CLine() = default;
public:
	virtual ~CLine() = default;

public:
	const _vec2&	Get_Point(const POINT& ePoint)	{ return m_vPoint[ePoint]; }
	_vec2*			Get_Point()						{ return m_vPoint; }
	_vec2&			Get_Normal()					{ return m_vNormal; }

	HRESULT Ready_Line(const _vec2* pPointA, const _vec2* pPointB);
	COMPARE Compare(const _vec2* pEndPos);

	void	Release();

public:
	static CLine* Create(const _vec2* pPointA, const _vec2* pPointB);

private:
	_vec2 m_vPoint[POINT_END];
	_vec2 m_vDirection;
	_vec2 m_vNormal;
};

