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
	int			g_iFrameCnt	: packoffset(c4.x);		// ��������Ʈ �̹��� X�� ����.
	int			g_iCurFrame : packoffset(c4.y);		// ���� �׷����� �̹����� X�� Index.
	int			g_iSceneCnt : packoffset(c4.z);		// ��������Ʈ �̹��� Y�� ����.
	int			g_iCurScene : packoffset(c4.w);		// ���� �׷����� �̹����� Y�� Index.
	
	// Texture Gauge.
	float		g_fGauge	: packoffset(c5.x);
	
}


/*____________________________________________________________________
[ Normal Texture X ]
______________________________________________________________________*/
struct VS_TEX_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_TEX_OUT
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
};

// VS_MAIN
VS_TEX_OUT VS_MAIN(VS_TEX_IN vs_input)
{
	VS_TEX_OUT vs_output = (VS_TEX_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

// PS_MAIN
float4 PS_MAIN(VS_TEX_OUT ps_input) : SV_TARGET
{
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	return (Color);
}



/*____________________________________________________________________
[ Normal Texture O ]
______________________________________________________________________*/
// VS_NORMAL_MAIN
struct VS_TEXNORMAL_IN
{
	float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 TexUV	: TEXCOORD;
};

struct VS_TEXNORMAL_OUT
{
	float4 Pos		: SV_POSITION;
	float3 Normal	: TEXCOORD0;
	float2 TexUV	: TEXCOORD1;
};


VS_TEXNORMAL_OUT VS_NORMAL_MAIN(VS_TEXNORMAL_IN vs_input)
{
	VS_TEXNORMAL_OUT vs_output = (VS_TEXNORMAL_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.Normal	= vs_input.Normal;
	vs_output.TexUV		= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_NORMAL_MAIN(VS_TEXNORMAL_OUT ps_input) : SV_TARGET
{
	float4 Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	float4 Normal	= float4(ps_input.Normal, 1.f);
	
	return (Diffuse);
}

/*__________________________________________________________________________________________________________
[ Texture Sprite ]
____________________________________________________________________________________________________________*/
VS_TEXNORMAL_OUT VS_TEXTURE_SPRITE(VS_TEXNORMAL_IN vs_input)
{
	VS_TEXNORMAL_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_TEXTURE_SPRITE(VS_TEXNORMAL_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / (float)g_iFrameCnt) + g_iCurFrame * (1.0f / (float)g_iFrameCnt);
	float v = (ps_input.TexUV.y / (float)g_iSceneCnt) + g_iCurScene * (1.0f / (float)g_iSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
	
	return (Color);
}


/*__________________________________________________________________________________________________________
[ Texture Guage ]
____________________________________________________________________________________________________________*/
VS_TEXNORMAL_OUT VS_GAUAGE(VS_TEXNORMAL_IN vs_input)
{
	VS_TEXNORMAL_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}

float4 PS_GAUAGE(VS_TEXNORMAL_OUT ps_input) : SV_TARGET
{
	float4 vDiffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	float fGauge	= ceil(g_fGauge - ps_input.TexUV.x);
	
	float4 vColor = vDiffuse * fGauge;
	
	return (vColor);
}