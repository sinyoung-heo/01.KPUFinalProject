#ifndef Engine_Struct_h__
#define Engine_Struct_h__

/*__________________________________________________________________________________________________________
[ VECTOR2 ]
____________________________________________________________________________________________________________*/
typedef struct tagVector2 : public XMFLOAT2
{
	explicit tagVector2() : XMFLOAT2(0.f, 0.f) {}
	explicit tagVector2(const float& x, const float& y) : XMFLOAT2(x, y) {}
	explicit tagVector2(const float& input) : XMFLOAT2(input, input) {}
	explicit tagVector2(const XMFLOAT2& input) : XMFLOAT2(input) {}

	/*____________________________________________________________________
	연산자 오버로딩
	______________________________________________________________________*/
	// 덧셈
	tagVector2 operator+(const tagVector2& input)				{ return tagVector2(this->x + input.x, this->y + input.y); }
	const tagVector2 operator+(const tagVector2& input) const	{ return tagVector2(this->x + input.x, this->y + input.y); }

	// 뺄셈
	tagVector2 operator-(const tagVector2& input)				{ return tagVector2(this->x - input.x, this->y - input.y); }
	const tagVector2 operator-(const tagVector2& input) const	{ return tagVector2(this->x - input.x, this->y - input.y); }

	// 곱셈
	tagVector2 operator*(const tagVector2& input)				{ return tagVector2(this->x * input.x, this->y * input.y); }
	tagVector2 operator*(const int& input)						{ return tagVector2((float)input * this->x, (float)input * this->y); }
	tagVector2 operator*(const float& input)					{ return tagVector2(input * this->x, input * this->y); }
	tagVector2 operator*(const double& input)					{ return tagVector2((float)input * this->x, (float)input * this->y); }
	const tagVector2 operator*(const tagVector2& input) const	{ return tagVector2(this->x * input.x, this->y * input.y); }
	const tagVector2 operator*(const int& input)		const	{ return tagVector2((float)input * this->x, (float)input * this->y); }
	const tagVector2 operator*(const float& input)		const	{ return tagVector2(input * this->x, input * this->y); }
	const tagVector2 operator*(const double& input)		const	{ return tagVector2((float)input * this->x, (float)input * this->y); }

	friend tagVector2 operator*(const int& input, tagVector2& v1)		{ return tagVector2((float)input * v1.x, (float)input * v1.y); }
	friend tagVector2 operator*(const float& input, tagVector2& v1)		{ return tagVector2(input * v1.x, input * v1.y); }
	friend tagVector2 operator*(const double& input, tagVector2& v1)	{ return tagVector2((float)input * v1.x, (float)input * v1.y); }

	// 축약 연산자
	tagVector2 operator+=(const tagVector2& input)	{ return (*this) = (*this) + input; }
	tagVector2 operator-=(const tagVector2& input)	{ return (*this) = (*this) - input; }
	tagVector2 operator*=(const tagVector2& input)	{ return (*this) = (*this) * input; }
	tagVector2 operator*=(const int& input)			{ return (*this) = (*this) * input; }
	tagVector2 operator*=(const float& input)		{ return (*this) = (*this) * input; }
	tagVector2 operator*=(const double& input)		{ return (*this) = (*this) * input; }

	// 비교 연산자
	bool operator==(const tagVector2& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, 0.f, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, 0.f, 0.f);
		return XMVector2Equal(dst, src);
	}

	bool operator!=(const tagVector2& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, 0.f, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, 0.f, 0.f);
		return !XMVector2Equal(dst, src);
	}

	/*____________________________________________________________________
	Function.
	______________________________________________________________________*/
	XMVECTOR Get_XMVECTOR()
	{
		return XMVectorSet(this->x, this->y, 0.f, 0.f);
	}

	void Normalize()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, 0.f, 0.f);
		temp = XMVector2Normalize(temp);

		this->x = XMVectorGetX(temp);
		this->y = XMVectorGetY(temp);
	}

	float Get_Length()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, 0.f, 0.f);
		temp = XMVector2Length(temp);

		return XMVectorGetX(temp);
	}

	float Get_Distance(const tagVector2& vDst)
	{
		XMVECTOR dst = XMVectorSet(vDst.x, vDst.y, 0.f, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, 0.f, 0.f);

		XMVECTOR result = dst - src;
		result = XMVector2Length(result);

		return XMVectorGetX(result);
	}

	float Get_Angle(const tagVector2& vDst)
	{
		XMVECTOR v1 = XMVectorSet(vDst.x, vDst.y, 0.f, 0.f);
		XMVECTOR v2 = XMVectorSet(this->x, this->y, 0.f, 0.f);

		XMVECTOR angle = XMVector2AngleBetweenVectors(v1, v2);
		float	angleRadians = XMVectorGetX(angle);

		return XMConvertToDegrees(angleRadians);
	}

	float Dot(const tagVector2& v1)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, 0.f, 0.f);
		XMVECTOR V2 = XMVectorSet(this->x, this->y, 0.f, 0.f);
		XMVECTOR result = XMVector2Dot(V1, V2);
		return XMVectorGetX(result);
	}

	tagVector2 Cross_InputV1(const tagVector2& v1)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, 0.f, 0.f);
		XMVECTOR V2 = XMVectorSet(this->x, this->y, 0.f, 0.f);
		XMVECTOR result = XMVector2Cross(V1, V2);

		return tagVector2(XMVectorGetX(result), XMVectorGetY(result));
	}

	tagVector2 Cross_InputV2(const tagVector2& v2)
	{
		XMVECTOR V1 = XMVectorSet(this->x, this->y, 0.f, 0.f);
		XMVECTOR V2 = XMVectorSet(v2.x, v2.y, 0.f, 0.f);
		XMVECTOR result = XMVector2Cross(V1, V2);

		return tagVector2(XMVectorGetX(result), XMVectorGetY(result));
	}

	void Print() { cout << "x : " << this->x << "\t y : " << this->y << endl; }

} _vec2;

/*__________________________________________________________________________________________________________
[ VECTOR3 ]
____________________________________________________________________________________________________________*/
typedef struct tagVector3 : public XMFLOAT3
{
	explicit tagVector3() : XMFLOAT3(0.f, 0.f, 0.f) {}
	explicit tagVector3(const float& x, const float& y, const float& z) : XMFLOAT3(x, y, z) {}
	explicit tagVector3(const float& input) : XMFLOAT3(input, input, input) {}
	explicit tagVector3(const XMFLOAT3& input) : XMFLOAT3(input) {}
	explicit tagVector3(const XMFLOAT4& input) : XMFLOAT3(input.x, input.y, input.z) {}
	explicit tagVector3(XMFLOAT3& input) : XMFLOAT3(input) {}
	explicit tagVector3(XMFLOAT4& input) : XMFLOAT3(input.x, input.y, input.z) {}

	/*____________________________________________________________________
	연산자 오버로딩
	______________________________________________________________________*/
	// 덧셈
	tagVector3 operator+(const tagVector3& input)				{ return tagVector3(this->x + input.x, this->y + input.y, this->z + input.z); }
	const tagVector3 operator+(const tagVector3& input) const	{ return tagVector3(this->x + input.x, this->y + input.y, this->z + input.z); }

	// 뺄셈
	tagVector3 operator-(const tagVector3& input)				{ return tagVector3(this->x - input.x, this->y - input.y, this->z - input.z); }
	const tagVector3 operator-(const tagVector3& input) const	{ return tagVector3(this->x - input.x, this->y - input.y, this->z - input.z); }

	// 곱셈
	tagVector3 operator*(const tagVector3& input)				{ return tagVector3(this->x * input.x, this->y * input.y, this->z * input.z); }
	tagVector3 operator*(const int& input)						{ return tagVector3((float)input * this->x, (float)input * this->y, (float)input * this->z); }
	tagVector3 operator*(const float& input)					{ return tagVector3(input * this->x, input * this->y, input * this->z); }
	tagVector3 operator*(const double& input)					{ return tagVector3((float)input * this->x, (float)input * this->y, (float)input * this->z); }
	const tagVector3 operator*(const tagVector3& input)	const	{ return tagVector3(this->x * input.x, this->y * input.y, this->z * input.z); }
	const tagVector3 operator*(const int& input)		const	{ return tagVector3((float)input * this->x, (float)input * this->y, (float)input * this->z); }
	const tagVector3 operator*(const float& input)		const	{ return tagVector3(input * this->x, input * this->y, input * this->z); }
	const tagVector3 operator*(const double& input)		const	{ return tagVector3((float)input * this->x, (float)input * this->y, (float)input * this->z); }

	friend tagVector3 operator*(const int& input, tagVector3& v1)		{ return tagVector3((float)input * v1.x, (float)input * v1.y, (float)input * v1.z); }
	friend tagVector3 operator*(const float& input, tagVector3& v1)		{ return tagVector3(input * v1.x, input * v1.y, input * v1.z); }
	friend tagVector3 operator*(const double& input, tagVector3& v1)	{ return tagVector3((float)input * v1.x, (float)input * v1.y, (float)input * v1.z); }

	// 나눗셉
	tagVector3 operator/(const int& input)					{ return tagVector3(this->x / (float)input, this->y / (float)input, this->z / (float)input); }
	tagVector3 operator/(const float& input)				{ return tagVector3(this->x / input, this->y / input, this->z / input); }
	tagVector3 operator/(const double& input)				{ return tagVector3(this->x / (float)input, this->y / (float)input, this->z / (float)input); }
	const tagVector3 operator/(const int& input)	const	{ return tagVector3(this->x / (float)input, this->y / (float)input, this->z / (float)input); }
	const tagVector3 operator/(const float& input)	const	{ return tagVector3(this->x / input, this->y / input, this->z / input); }
	const tagVector3 operator/(const double& input) const	{ return tagVector3(this->x / (float)input, this->y / (float)input, this->z / (float)input); }

	// 축약 연산자
	tagVector3 operator+=(const tagVector3& input)	{ return (*this) = (*this) + input; }
	tagVector3 operator-=(const tagVector3& input)	{ return (*this) = (*this) - input; }
	tagVector3 operator*=(const tagVector3& input)	{ return (*this) = (*this) * input; }
	tagVector3 operator*=(const int& input)			{ return (*this) = (*this) * input; }
	tagVector3 operator*=(const float& input)		{ return (*this) = (*this) * input; }
	tagVector3 operator*=(const double& input)		{ return (*this) = (*this) * input; }

	// 비교 연산자
	bool operator==(const tagVector3& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, input.z, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, 0.f);
		return XMVector3Equal(dst, src);
	}

	bool operator!=(const tagVector3& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, input.z, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, 0.f);
		return !XMVector3Equal(dst, src);
	}

	/*____________________________________________________________________
	Function.
	______________________________________________________________________*/
	XMVECTOR Get_XMVECTOR()
	{
		return XMVectorSet(this->x, this->y, this->z, 0.f);
	}

	void Normalize()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, this->z, 0.f);
		temp = XMVector3Normalize(temp);

		this->x = XMVectorGetX(temp);
		this->y = XMVectorGetY(temp);
		this->z = XMVectorGetZ(temp);
	}

	float Get_Length()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, this->z, 0.f);
		temp = XMVector3Length(temp);

		return XMVectorGetX(temp);
	}

	float Get_Distance(const tagVector3& vDst)
	{
		XMVECTOR dst = XMVectorSet(vDst.x, vDst.y, vDst.z, 0.f);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, 0.f);

		XMVECTOR result = dst - src;
		result = XMVector3Length(result);

		return XMVectorGetX(result);
	}

	float Get_Angle(const tagVector3& vDst)
	{
		XMVECTOR v1 = XMVectorSet(vDst.x, vDst.y, vDst.z, 0.f);
		XMVECTOR v2 = XMVectorSet(this->x, this->y, this->z, 0.f);

		XMVECTOR angle = XMVector3AngleBetweenVectors(v1, v2);
		float	angleRadians = XMVectorGetX(angle);

		float fRet = 0.f;

		if (this->z < 0.f)
		{
			fRet = XMConvertToDegrees(angleRadians);

			if (this->x < 0.0f)
				return 360.f - fRet;			
			else				
				return fRet;
			
		}
		else
		{
			fRet = XMConvertToDegrees(angleRadians);
			if (this->x < 0.0f)
				return 360.f - fRet;		
			else
				return fRet;
		}
	}

	float Dot(const tagVector3& v1)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, v1.z, 0.f);
		XMVECTOR V2 = XMVectorSet(this->x, this->y, this->z, 0.f);
		XMVECTOR result = XMVector3Dot(V1, V2);
		return XMVectorGetX(result);
	}

	tagVector3 Cross_InputV1(const tagVector3& v1)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, v1.z, 0.f);
		XMVECTOR V2 = XMVectorSet(this->x, this->y, this->z, 0.f);
		XMVECTOR result = XMVector3Cross(V1, V2);

		return tagVector3(XMVectorGetX(result), XMVectorGetY(result), XMVectorGetZ(result));
	}

	tagVector3 Cross_InputV2(const tagVector3& v2)
	{
		XMVECTOR V1 = XMVectorSet(this->x, this->y, this->z, 0.f);
		XMVECTOR V2 = XMVectorSet(v2.x, v2.y, v2.z, 0.f);
		XMVECTOR result = XMVector3Cross(V1, V2);

		return tagVector3(XMVectorGetX(result), XMVectorGetY(result), XMVectorGetZ(result));
	}

	void TransformCoord(const tagVector3& input, const XMMATRIX& matrix)
	{
		XMVECTOR vIn	= XMVectorSet(input.x, input.y, input.z, 0.f);
		XMVECTOR vOut	= XMVector3TransformCoord(vIn, matrix);

		this->x = XMVectorGetX(vOut);
		this->y = XMVectorGetY(vOut);
		this->z = XMVectorGetZ(vOut);
	}

	void TransformNormal(const tagVector3& input, const XMMATRIX& matrix)
	{
		XMVECTOR vIn	= XMVectorSet(input.x, input.y, input.z, 0.f);
		XMVECTOR vOut	= XMVector3TransformNormal(vIn, matrix);

		this->x = XMVectorGetX(vOut);
		this->y = XMVectorGetY(vOut);
		this->z = XMVectorGetZ(vOut);
	}

	tagVector3 Convert_2DWindowToDescartes(const int& WINCX, const int& WINCY)
	{
		/*____________________________________________________________________
		2D UI 사용 시, Window좌표를 입력해서 사용할 수 있도록
		윈도우 좌표를 데카르트 좌표로 변환해주는 좌표.
		______________________________________________________________________*/
		tagVector3 vDescartes;

		vDescartes.x =  this->x - float(WINCX) / 2.f;
		vDescartes.y = -this->y + float(WINCY) / 2.f;
		vDescartes.z =  0.f;

		return vDescartes;
	}

	tagVector3 Convert_ProjectionToScreen(const XMMATRIX& matView, const XMMATRIX& matProj, const D3D12_VIEWPORT& vp)
	{
		/*____________________________________________________________________
		3D 투영 좌표 -> 2D 스크린 좌표로 변환.
		______________________________________________________________________*/
		// 1. 월드 좌표 -> 뷰 스페이스 변환.
		tagVector3 vPos_WV;
		vPos_WV.TransformCoord(*this, matView);

		// 2. 뷰 스페이스 -> 투영 변환.
		tagVector3 vPos_WVP;
		vPos_WVP.TransformCoord(vPos_WV, matProj);

		// 3. 투영 변환 -> 뷰포트 변환. (Screen 좌표 변환)
		tagVector3 vScreen;

		vScreen.x = vPos_WVP.x * (vp.Width / 2) + (vp.TopLeftX + vp.Width / 2);
		vScreen.y = vPos_WVP.y * (-1.f * vp.Height / 2) + (vp.TopLeftY + vp.Height / 2);
		vScreen.z = 0.f;

		return vScreen;
	}

	void Print() { cout << "x : " << this->x << "\t y : " << this->y << "\t z : " << this->z << endl; }

} _vec3, _rgb;

const _vec3	g_vLook = _vec3(0.f, 0.f, 1.f);

/*__________________________________________________________________________________________________________
[ VECTOR4 ]
____________________________________________________________________________________________________________*/
typedef struct tagVector4 : public XMFLOAT4
{
	explicit tagVector4() : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
	explicit tagVector4(const float& x, const float& y, const float& z, const float& w) : XMFLOAT4(x, y, z, w) {}
	explicit tagVector4(const float& input) : XMFLOAT4(input, input, input, input) {}
	explicit tagVector4(const XMFLOAT4& input) : XMFLOAT4(input) {}
	explicit tagVector4(const XMFLOAT3& vector, const float& w) : XMFLOAT4(vector.x, vector.y, vector.z, w) {}
	explicit tagVector4(XMFLOAT3& vector, float& w) : XMFLOAT4(vector.x, vector.y, vector.z, w) {}

	/*____________________________________________________________________
	연산자 오버로딩
	______________________________________________________________________*/
	// 덧셈
	tagVector4 operator+(const tagVector4& input)				{ return tagVector4(this->x + input.x, this->y + input.y, this->z + input.z, this->w + input.w); }
	const tagVector4 operator+(const tagVector4& input) const	{ return tagVector4(this->x + input.x, this->y + input.y, this->z + input.z, this->w + input.w); }

	// 뺄셈
	tagVector4 operator-(const tagVector4& input)				{ return tagVector4(this->x - input.x, this->y - input.y, this->z - input.z, this->w - input.w); }
	const tagVector4 operator-(const tagVector4& input) const	{ return tagVector4(this->x - input.x, this->y - input.y, this->z - input.z, this->w - input.w); }

	// 곱셈
	tagVector4 operator*(const tagVector4& input)				{ return tagVector4(this->x * input.x, this->y * input.y, this->z * input.z, this->w * input.w); }
	tagVector4 operator*(const int& input)						{ return tagVector4((float)input * this->x, (float)input * this->y, (float)input * this->z, (float)input * this->w); }
	tagVector4 operator*(const float& input)					{ return tagVector4(input * this->x, input * this->y, input * this->z, input * this->w); }
	tagVector4 operator*(const double& input)					{ return tagVector4((float)input * this->x, (float)input * this->y, (float)input * this->z, (float)input * this->w); }
	const tagVector4 operator*(const tagVector4& input)	const	{ return tagVector4(this->x * input.x, this->y * input.y, this->z * input.z, this->w * input.w); }
	const tagVector4 operator*(const int& input)		const	{ return tagVector4((float)input * this->x, (float)input * this->y, (float)input * this->z, (float)input * this->w); }
	const tagVector4 operator*(const float& input)		const	{ return tagVector4(input * this->x, input * this->y, input * this->z, input * this->w); }
	const tagVector4 operator*(const double& input)		const	{ return tagVector4((float)input * this->x, (float)input * this->y, (float)input * this->z, (float)input * this->w); }

	friend tagVector4 operator*(const int& input, tagVector4& v1)		{ return tagVector4((float)input * v1.x, (float)input * v1.y, (float)input * v1.z, (float)input * v1.w); }
	friend tagVector4 operator*(const float& input, tagVector4& v1)		{ return tagVector4(input * v1.x, input * v1.y, input * v1.z, input * v1.w); }
	friend tagVector4 operator*(const double& input, tagVector4& v1)	{ return tagVector4((float)input * v1.x, (float)input * v1.y, (float)input * v1.z, (float)input * v1.w); }

	// 축약 연산자
	tagVector4 operator+=(const tagVector4& input)	{ return (*this) = (*this) + input; }
	tagVector4 operator-=(const tagVector4& input)	{ return (*this) = (*this) - input; }
	tagVector4 operator*=(const tagVector4& input)	{ return (*this) = (*this) * input; }
	tagVector4 operator*=(const int& input)			{ return (*this) = (*this) * input; }
	tagVector4 operator*=(const float& input)		{ return (*this) = (*this) * input; }
	tagVector4 operator*=(const double& input)		{ return (*this) = (*this) * input; }

	// 비교 연산자
	bool operator==(const tagVector4& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, input.z, input.w);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, this->w);
		return XMVector4Equal(dst, src);
	}

	bool operator!=(const tagVector4& input)
	{
		XMVECTOR dst = XMVectorSet(input.x, input.y, input.z, input.w);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, this->w);
		return !XMVector4Equal(dst, src);
	}

	/*____________________________________________________________________
	Function.
	______________________________________________________________________*/
	XMVECTOR Get_XMVECTOR()
	{
		return XMVectorSet(this->x, this->y, this->z, this->w);
	}

	void Normalize()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, this->z, this->w);
		temp = XMVector4Normalize(temp);

		this->x = XMVectorGetX(temp);
		this->y = XMVectorGetY(temp);
		this->z = XMVectorGetZ(temp);
		this->w = XMVectorGetW(temp);
	}

	float Get_Length()
	{
		XMVECTOR temp = XMVectorSet(this->x, this->y, this->z, this->w);
		temp = XMVector4Length(temp);

		return XMVectorGetX(temp);
	}

	float Get_Distance(const tagVector4& vDst)
	{
		XMVECTOR dst = XMVectorSet(vDst.x, vDst.y, vDst.z, vDst.w);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, this->w);

		XMVECTOR result = dst - src;
		result = XMVector4Length(result);

		return XMVectorGetX(result);
	}

	float Get_Angle(const tagVector4& vDst)
	{
		XMVECTOR v1 = XMVectorSet(this->x, this->y, this->z, this->w);
		XMVECTOR v2 = XMVectorSet(vDst.x, vDst.y, vDst.z, vDst.w);

		XMVECTOR angle = XMVector4AngleBetweenVectors(v1, v2);
		float	angleRadians = XMVectorGetX(angle);

		return XMConvertToDegrees(angleRadians);
	}

	float Dot(const tagVector4& vDst)
	{
		XMVECTOR dst = XMVectorSet(vDst.x, vDst.y, vDst.z, vDst.w);
		XMVECTOR src = XMVectorSet(this->x, this->y, this->z, this->w);
		XMVECTOR result = XMVector4Dot(dst, src);
		return XMVectorGetX(result);
	}

	tagVector4 Cross_Input_V1_V2(const tagVector4& v1, const tagVector4& v2)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, v1.z, v1.w);
		XMVECTOR V2 = XMVectorSet(v2.x, v2.y, v2.z, v2.w);
		XMVECTOR V3 = XMVectorSet(this->x, this->y, this->z, this->w);
		XMVECTOR result = XMVector4Cross(V1, V2, V3);

		return tagVector4(XMVectorGetX(result), XMVectorGetY(result), XMVectorGetZ(result), XMVectorGetW(result));
	}

	tagVector4 Cross_Input_V2_V3(const tagVector4& v2, const tagVector4& v3)
	{
		XMVECTOR V1 = XMVectorSet(this->x, this->y, this->z, this->w);
		XMVECTOR V2 = XMVectorSet(v2.x, v2.y, v2.z, v2.w);
		XMVECTOR V3 = XMVectorSet(v3.x, v3.y, v3.z, v3.w);
		XMVECTOR result = XMVector4Cross(V1, V2, V3);

		return tagVector4(XMVectorGetX(result), XMVectorGetY(result), XMVectorGetZ(result), XMVectorGetW(result));
	}

	tagVector4 Cross_Input_V1_V3(const tagVector4& v1, const tagVector4& v3)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, v1.z, v1.w);
		XMVECTOR V2 = XMVectorSet(this->x, this->y, this->z, this->w);
		XMVECTOR V3 = XMVectorSet(v3.x, v3.y, v3.z, v3.w);
		XMVECTOR result = XMVector4Cross(V1, V2, V3);

		return tagVector4(XMVectorGetX(result), XMVectorGetY(result), XMVectorGetZ(result), XMVectorGetW(result));
	}

	// Plane
	void Plane_FromPoints(const _vec3& v1, const _vec3& v2, const _vec3& v3)
	{
		XMVECTOR V1 = XMVectorSet(v1.x, v1.y, v1.z, 1.f);
		XMVECTOR V2 = XMVectorSet(v2.x, v2.y, v2.z, 1.f);
		XMVECTOR V3 = XMVectorSet(v3.x, v3.y, v3.z, 1.f);

		XMVECTOR P1 = XMPlaneFromPoints(V1, V2, V3);

		this->x = XMVectorGetX(P1);
		this->y = XMVectorGetY(P1);
		this->z = XMVectorGetZ(P1);
		this->w = XMVectorGetW(P1);
	}

	void Print() { cout << "x : " << this->x << "\t y : " << this->y << "\t z : " << this->z << "\t w : " << this->w << endl; }

} _vec4, _rgba, _rect, _plane;


namespace Engine
{
	/*__________________________________________________________________________________________________________
	[ CAMERA DESC ]
	____________________________________________________________________________________________________________*/
	typedef struct tagCameraDesc
	{
		tagCameraDesc()
			: vEye(_vec3(0.f, 0.f, 0.f)), vAt(_vec3(0.f, 0.f, 0.f)), vUp(_vec3(0.f, 0.f, 0.f)), matView(XMMatrixIdentity()) {}
		tagCameraDesc(_vec3 eye, _vec3 at, _vec3 up)
			: vEye(eye) ,vAt(at), vUp(up), matView(XMMatrixIdentity()) {}
		tagCameraDesc(const tagCameraDesc& input)
			: vEye(input.vEye), vAt(input.vAt), vUp(input.vUp), matView(input.matView) {}
	
		_vec3	vEye;
		_vec3	vAt;
		_vec3	vUp;
		_matrix	matView;
	
	} CAMERA_DESC;
	
	typedef struct tagPerspectiveDesc
	{
		tagPerspectiveDesc()
			: fFovY(0.0f), fAspect(0.0f), fNearZ(0.0f), fFarZ(0.0f), matProj(XMMatrixIdentity()) {}
		tagPerspectiveDesc(_float _fovY, _float _aspect, _float _near = 1.0f, _float _far = 1000.f)
			: fFovY(_fovY), fAspect(_aspect), fNearZ(_near), fFarZ(_far), matProj(XMMatrixIdentity()) {}
		tagPerspectiveDesc(const tagPerspectiveDesc& input)
			: fFovY(input.fFovY), fAspect(input.fAspect), fNearZ(fNearZ), fFarZ(input.fFarZ), matProj(input.matProj) {}
	
		_float	fFovY;
		_float	fAspect;
		_float	fNearZ;
		_float	fFarZ;
		_matrix	matProj;
	
	} PROJ_DESC;
	
	typedef struct tagOrthographicDesc
	{
		tagOrthographicDesc()
			: fWidth(0.0f), fHeight(0.0f), fNearZ(0.0f), fFarZ(0.0f), matProj(XMMatrixIdentity()) {}
		tagOrthographicDesc(_float _width, _float _height, _float _near = 0.f, _float _far = 1.f)
			: fWidth(_width), fHeight(_height), fNearZ(_near), fFarZ(_far), matProj(XMMatrixIdentity()) {}
		tagOrthographicDesc(const tagOrthographicDesc& input)
			: fWidth(input.fWidth), fHeight(input.fHeight), fNearZ(input.fNearZ), fFarZ(input.fFarZ), matProj(input.matProj) {}
	
		_float	fWidth;
		_float	fHeight;
		_float	fNearZ;
		_float	fFarZ;
		_matrix matProj;
	
	} ORTHO_DESC;


	/*__________________________________________________________________________________________________________
	[ VERTEX ]
	____________________________________________________________________________________________________________*/
	typedef struct tagVertexColor
	{
		tagVertexColor() = default;
		tagVertexColor(const _vec3& pos, const _rgba& color) 
			: vPos(pos), vColor(color) {}

		_vec3 vPos;	 
		_rgba vColor;

	} VTXCOL;

	typedef struct tagVertexTexture
	{
		tagVertexTexture() = default;
		tagVertexTexture(const _vec3& pos, const _vec2& uv) 
			: vPos(pos), vTexUV(uv) {}

		_vec3 vPos;
		_vec2 vTexUV;

	} VTXTEX;

	typedef struct tagVertexCube
	{
		tagVertexCube() = default;
		tagVertexCube(const _vec3& pos, const _vec3& tex) : vPos(pos), vTex(tex) {}

		_vec3 vPos;
		_vec3 vTex;

	} VTXCUBE;

	typedef struct tagVertexScreen
	{
		tagVertexScreen() = default;
		tagVertexScreen(const _vec3& pos, const _vec2&uv) 
			: vPos(pos), vTexUV(uv) {}

		_vec3 vPos;
		_vec2 vTexUV;

	} VTXSCREEN;

	typedef struct tagVertexMesh
	{
		tagVertexMesh() = default;
		tagVertexMesh(const _vec3& pos, const _vec3& normal, const _vec2& uv, const _vec4* pBoneID, const _vec4* pBoneWeight)
			: vPos(pos), vNormal(normal), vTexUV(uv)
		{
			for (int i = 0; i < 2; ++i)
			{
				arrBoneID[i]		= pBoneID[i];
				arrBoneWeight[i]	= pBoneWeight[i];
			}
		}

		_vec3 vPos;
		_vec3 vNormal;
		_vec2 vTexUV;

		_vec4 arrBoneID[2];
		_vec4 arrBoneWeight[2];

	} VTXMESH;


	/*__________________________________________________________________________________________________________
	[ Mesh ]
	____________________________________________________________________________________________________________*/
	typedef struct tagMeshEntry 
	{
		tagMeshEntry() 
			: NumIndices(0), BaseVertex(0), BaseIndex(0), MaterialIndex(0xFFFFFFFF), blsTexture(false) {}

		_uint NumIndices;
		_uint BaseVertex;
		_uint BaseIndex;
		_uint MaterialIndex;
		_bool blsTexture;

	} MESH_ENTRY;


	/*__________________________________________________________________________________________________________
	[ Mesh Animation Matrix ]
	____________________________________________________________________________________________________________*/
	typedef struct tagBoneDesc
	{
		_matrix matBoneOffset		= INIT_MATRIX;	// 뼈가 갖고있는 로컬 정보.
		_matrix matFinalTransform	= INIT_MATRIX;	// 뼈 행렬의 최종 변환 행렬.

	} BONE_DESC;

	typedef struct tagSkinningMatrix
	{
		_matrix	matBoneOffset		= INIT_MATRIX;
		_matrix matBoneScale		= INIT_MATRIX;
		_matrix matBoneRotation		= INIT_MATRIX;
		_matrix matBoneTrans		= INIT_MATRIX;
		_matrix matParentTransform	= INIT_MATRIX;
		_matrix matRootTransform	= INIT_MATRIX;

	} SKINNING_MATRIX;

	/*__________________________________________________________________________________________________________
	[ SUBMESH_GEOMETRY ]
	____________________________________________________________________________________________________________*/
	typedef struct tagSubmeshGeometry
	{
		_uint	uiIndexCount			= 0;
		_uint	uiStartIndexLocation	= 0;
		_int	iBaseVertexLocation		= 0;

	} SUBMESH_GEOMETRY;

	/*__________________________________________________________________________________________________________
	[ Light Info ]
	____________________________________________________________________________________________________________*/
	typedef struct tagD3DLight
	{
		LIGHTTYPE	Type		= D3DLIGHT_DIRECTIONAL;
		_rgba		Diffuse		= _rgba(0.0f, 0.0f, 0.0f, 0.0f);
		_rgba		Specular	= _rgba(0.0f, 0.0f, 0.0f, 0.0f);
		_rgba		Ambient		= _rgba(0.0f, 0.0f, 0.0f, 0.0f);
		_vec4		Position	= _rgba(0.0f, 0.0f, 0.0f, 0.0f);
		_vec4		Direction	= _rgba(0.0f, 0.0f, 0.0f, 0.0f);
		_float		Range		= 0.0f;

	} D3DLIGHT;

	/*__________________________________________________________________________________________________________
	[ ShadowInfo ]
	____________________________________________________________________________________________________________*/
	typedef struct tagShadowDesc
	{
		_matrix		matLightView;
		_matrix		matLightProj;
		_vec4		vLightPosition;
		_float		fLightPorjFar;

	} SHADOW_DESC;


	/*__________________________________________________________________________________________________________
	[ InstancingDesc ]
	____________________________________________________________________________________________________________*/
	typedef struct tagInstancingDesc
	{
		_uint		iInstanceCount = 0;

	} INSTANCING_DESC;


	/*__________________________________________________________________________________________________________
	[ Shader ConstantBuffer Struct ]
	____________________________________________________________________________________________________________*/
	// Camera Matrix
	typedef struct tagConstantBufferCameraDesc
	{
		XMFLOAT4X4	matView;
		XMFLOAT4X4	matProj;
		_vec4		vCameraPos;
		_float		fProjFar;

	} CB_CAMERA_MATRIX;


	// Shader Color
	typedef struct tagConstantBufferShaderColor
	{
		XMFLOAT4X4	matWorld;
		_rgba		vColor;

	} CB_SHADER_COLOR;


	// Shader Texture
	typedef struct tagConstantBufferShaderTexture
	{
		XMFLOAT4X4	matWorld;

		_int		iFrameCnt;	// 스프라이트 이미지 X축 개수.
		_int		iCurFrame;	// 현재 그려지는 이미지의 X축 Index.
		_int		iSceneCnt;	// 스프라이트 이미지 Y축 개수.
		_int		iCurScene;	// 현재 그려지는 이미지의 Y축 Index.

		_float		fGauge;
		_float		fOffset1;
		_float		fOffset2;
		_float		fOffset3;

	} CB_SHADER_TEXTURE;
	

	// Shader SkyBox
	typedef struct tagConstantBufferShaderSkyBox
	{
		XMFLOAT4X4	matWorld;

	} CB_SHADER_SKYBOX;


	// Shader Mesh
	typedef struct tagConstantBufferShaderMesh
	{
		XMFLOAT4X4	matWorld;

		XMFLOAT4X4	matLightView;
		XMFLOAT4X4	matLightProj;
		_vec4		vLightPos;
		_float		fLightPorjFar;
		_float		fDeltaTime;
		_float		fOffset1;
		_float		fOffset2;

	} CB_SHADER_MESH;

	typedef struct tagCBuffer
	{
		_vec4 vFloat4;
	}CB_SHADER_VARIABLE;

	typedef struct tagShaderInformation
	{
		_float Ssao;
		_float DepthOfField;
		_float Shade;
		_float ToneMapping;
		_float Distortion;
		_float Specular;
		_float Offset2;
		_float Offset3;
	}CB_SHADER_INFORMATION;
	// Shader Shadow
	typedef struct tagConstantBufferShaderShadow
	{
		XMFLOAT4X4	matWorld;
		XMFLOAT4X4	matView;
		XMFLOAT4X4	matProj;
		_float		fProjFar;
		_float		fOffset1;
		_float		fOffset2;
		_float		fOffset3;

	} CB_SHADER_SHADOW;

	typedef struct tagConstnatBufferSkinningMatrix
	{
		XMFLOAT4X4	matBoneOffset[64];
		XMFLOAT4X4	matBoneScale[64];
		XMFLOAT4X4	matBoneRotation[64];
		XMFLOAT4X4	matBoneTrans[64];
		XMFLOAT4X4	matParentTransform[64];
		XMFLOAT4X4	matRootTransform[64];

	} CB_SKINNING_MATRIX;

	// Shader Lighting
	typedef struct tagConstantBufferShaderLighting
	{
		XMFLOAT4X4	matViewInv;
		XMFLOAT4X4	matProjInv;
		_vec4		vCameraPos;
		_vec4		vProjFar;

		_rgba		vLightDiffuse;
		_rgba		vLightSpecular;
		_rgba		vLightAmbient;
		_vec4		vLightDir;
		_vec4		vLightPos;
		_vec4		vLightRange;

	} CB_SHADER_LIGHTING;
}



#endif // Engine_Struct_h__
