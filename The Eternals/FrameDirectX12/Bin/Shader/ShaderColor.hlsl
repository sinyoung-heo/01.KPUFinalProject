
/*____________________________________________________________________
- register는 상수버퍼와 리소들을 등록하는시멘틱. 
숫자는 옵션.
b - 상수버퍼
t - 텍스처
s - 샘플러

- packoffset은 한 번에 부동소수점 4개를 처리할 수 있는 
GPU를 고려해서 상수버퍼의 맴버들을 묶는 옵션이다.
- float3 과 float 을 이 옵션으로 묶을 수 있다.
______________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView		: packoffset(c0);
	float4x4	g_matProj		: packoffset(c4);
	float4		g_vCameraPos	: packoffset(c8);
	float		g_fProjFar		: packoffset(c9);
}

cbuffer cbShaderColor : register(b1)
{
	float4x4	g_matWorld	: packoffset(c0);
	float4		g_Color		: packoffset(c4);
}


/*__________________________________________________________________________________________________________
[ Vertex Shader ]
____________________________________________________________________________________________________________*/
struct VS_IN
{
	float3 Pos		: POSITION;
	float4 Color	: COLOR0;
};

struct VS_OUT
{
	float4 Pos		: SV_POSITION;
	float4 Color	: COLOR0;
};

// VS Input Color
VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.Color		= g_Color;
	
	return (vs_output);
}

// VS Random Color
VS_OUT VS_MAIN_RANDOM(VS_IN vs_input)
{
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.Color		= vs_input.Color;
	
	return (vs_output);
}

/*__________________________________________________________________________________________________________
[ Pixel Shader ]
____________________________________________________________________________________________________________*/
struct PS_OUT
{
	float4 Diffuse : SV_TARGET0; // 0번 RenderTarget - Diffuse
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	ps_output.Diffuse = ps_input.Color;
	
	return (ps_output);
}
