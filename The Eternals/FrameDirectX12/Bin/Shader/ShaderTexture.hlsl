/*____________________________________________________________________
[ Sampler ]
______________________________________________________________________*/
SamplerState g_samPointWrap			: register(s0);
SamplerState g_samPointClamp		: register(s1);
SamplerState g_samLinearWrap		: register(s2);
SamplerState g_samLinearClamp		: register(s3);
SamplerState g_samAnisotropicWrap	: register(s4);
SamplerState g_samAnisotropicClamp	: register(s5);

/*____________________________________________________________________
[ Texture ]
______________________________________________________________________*/
Texture2D g_TexDiffuse : register(t0);

/*____________________________________________________________________
[ Constant Buffer ]
______________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView		: packoffset(c0);
	float4x4	g_matProj		: packoffset(c4);
	float4		g_vCameraPos	: packoffset(c8);
	float		g_fProjFar		: packoffset(c9);
}

cbuffer cbShaderTexture : register(b1)
{
	float4x4	g_matWorld	: packoffset(c0);
	
	// Texture Sprite.
	float		g_fFrameCnt	: packoffset(c4.x);		// 스프라이트 이미지 X축 개수.
	float		g_fCurFrame : packoffset(c4.y);		// 현재 그려지는 이미지의 X축 Index.
	float		g_fSceneCnt : packoffset(c4.z);		// 스프라이트 이미지 Y축 개수.
	float		g_fCurScene : packoffset(c4.w);		// 현재 그려지는 이미지의 Y축 Index.
	
	// Texture Gauge.
	float		g_fGauge	: packoffset(c5.x);
	float		g_fAlpha	: packoffset(c5.y);
	float		fOffset2	: packoffset(c5.z);
	float		fOffset3	: packoffset(c5.w);
	//Color
    float4		g_vColor : packoffset(c6);
    float4 vWorld1 : packoffset(c7);
    float4 vWorld2 : packoffset(c8);
    float4 vWorld3 : packoffset(c9);
    float4 vWorld4 : packoffset(c10);
}

float2 rotateUV(float2 uv, float degrees)
{
    const float Deg2Rad = (3.14 * 2.0) / 360.0; //1도의 라디안값을 구한다
    float rotationRadians = degrees * Deg2Rad; //원하는 각도(디그리)의 라디안 값을 구한다
    float s = sin(rotationRadians);
    float c = cos(rotationRadians);
    float2x2 rotationMatrix = float2x2(c, -s, s, c); //회전 2차원 행렬을 만든다
    uv -= 0.5; //중심축을 이동시켜 가운데로 만들고
    uv = mul(rotationMatrix, uv); //회전한 다음에
    uv += 0.5; //다시 중심축 이동시켜 제자리로 만든다
    return uv;
}


struct VS_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_OUT
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
};
VS_OUT VS_PARTICLE(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;

    float4x4 matWorld = float4x4(vWorld1, vWorld2, vWorld3, vWorld4);
    float4x4 matWV, matWVP;
    matWV = mul(matWorld, g_matView);
    matWVP = mul(matWV, g_matProj);
	
	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
	
    return (vs_output);
}

VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	return (Color);
}

float4 PS_MAIN_ALPHA(VS_OUT ps_input) : SV_TARGET
{
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	Color.a *= g_fAlpha;
	
	return (Color);
}


/*__________________________________________________________________________________________________________
[ Texture Sprite ]
____________________________________________________________________________________________________________*/
VS_OUT VS_TEXTURE_SPRITE(VS_IN vs_input)
{
	VS_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_TEXTURE_SPRITE(VS_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
	float v = (ps_input.TexUV.y / g_fSceneCnt) + g_fCurScene * (1.0f / g_fSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
    //Color += mul(Color, fOffset2);
    Color.r += g_vColor.r;
    Color.g += g_vColor.g;
    Color.b += g_vColor.b;
	return (Color);
}

float4 PS_TEXTURE_SPRITE_ALPHA(VS_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
	float v = (ps_input.TexUV.y / g_fSceneCnt) + g_fCurScene * (1.0f / g_fSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
	Color.a *= g_fAlpha;
	
	return (Color);
}

float4 PS_TEXTURE_LIGHTING(VS_OUT ps_input) : SV_TARGET
{
    float u = (ps_input.TexUV.x / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
    float v = (ps_input.TexUV.y / g_fSceneCnt) + g_fCurScene * (1.0f / g_fSceneCnt);
	
    float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
    //Color.r += g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v)).r;
    //Color.g += g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v)).g;
    //Color.b += g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v)).b;
	
    return (Color);
}
/*__________________________________________________________________________________________________________
[ Texture Guage ]
____________________________________________________________________________________________________________*/
VS_OUT VS_GAUAGE(VS_IN vs_input)
{
	VS_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_GAUAGE(VS_OUT ps_input) : SV_TARGET
{
	float4 vDiffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	float fGauge	= ceil(g_fGauge - ps_input.TexUV.x);
	
	float4 vColor = vDiffuse * fGauge;
	
	return (vColor);
}

float4 PS_GAUAGE_SPRITE(VS_OUT ps_input) : SV_TARGET
{
	float u        = (ps_input.TexUV.x / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
	float v        = (ps_input.TexUV.y / g_fSceneCnt) + g_fCurScene * (1.0f / g_fSceneCnt);
	float fPercent = (g_fGauge / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
	
	float4	vDiffuse = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
	float	fGauge   = ceil(fPercent - u);
	float4	vColor   = vDiffuse * fGauge;
	
	return (vColor);
}

float4 PS_SNOW(VS_OUT ps_input) : SV_TARGET
{
    float u = (ps_input.TexUV.x / g_fFrameCnt) + g_fCurFrame * (1.0f / g_fFrameCnt);
    float v = (ps_input.TexUV.y / g_fSceneCnt) + g_fCurScene * (1.0f / g_fSceneCnt);
	
    float2 uv = rotateUV(float2(u, v), fOffset3);
    float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(uv.x, uv.y));
    Color += g_vColor;
    Color.a = g_fAlpha;
    return (Color);
}
