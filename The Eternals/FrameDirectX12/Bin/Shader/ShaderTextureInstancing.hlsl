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

/*__________________________________________________________________________________________________________
[ Shader Resource ]
____________________________________________________________________________________________________________*/
typedef struct tagShaderMesh
{
	float4x4	matWorld;
	
	// Texture Sprite.
	int			iFrameCnt;	// 스프라이트 이미지 X축 개수.
	int			iCurFrame;	// 현재 그려지는 이미지의 X축 Index.
	int			iSceneCnt;	// 스프라이트 이미지 Y축 개수.
	int			iCurScene;	// 현재 그려지는 이미지의 Y축 Index.
	
	// Texture Gauge.
	float		fGauge;
	float		fOffset1;
	float		fOffset2;
	float		fOffset3;
	
} SHADER_TEXTURE;
StructuredBuffer<SHADER_TEXTURE> g_ShaderTexture : register(t0, space1);

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

struct VS_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_OUT
{
	float4	Pos			: SV_POSITION;
	float2	TexUV		: TEXCOORD;
	
	int		iFrameCnt	: TEXCOORD1;
	int		iCurFrame	: TEXCOORD2;
	int		iSceneCnt	: TEXCOORD3;
	int		iCurScene	: TEXCOORD4;
	float	fGauge		: TEXCOORD5;
};


VS_OUT VS_MAIN(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_ShaderTexture[iInstanceID].matWorld, g_matView);
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


/*__________________________________________________________________________________________________________
[ Texture Sprite ]
____________________________________________________________________________________________________________*/
VS_OUT VS_TEXTURE_SPRITE(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_ShaderTexture[iInstanceID].matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	vs_output.iFrameCnt = g_ShaderTexture[iInstanceID].iFrameCnt;
	vs_output.iCurFrame = g_ShaderTexture[iInstanceID].iCurFrame;
	vs_output.iSceneCnt = g_ShaderTexture[iInstanceID].iSceneCnt;
	vs_output.iCurScene = g_ShaderTexture[iInstanceID].iCurScene;
	
	return (vs_output);
}

float4 PS_TEXTURE_SPRITE(VS_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / (float)ps_input.iFrameCnt) + ps_input.iCurFrame * (1.0f / (float)ps_input.iFrameCnt);
	float v = (ps_input.TexUV.y / (float)ps_input.iSceneCnt) + ps_input.iCurScene * (1.0f / (float)ps_input.iSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
	
	return (Color);
}


/*__________________________________________________________________________________________________________
[ Texture Guage ]
____________________________________________________________________________________________________________*/
VS_OUT VS_GAUAGE(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_ShaderTexture[iInstanceID].matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV		= vs_input.TexUV;
	vs_output.fGauge	= g_ShaderTexture[iInstanceID].fGauge;
	
	return (vs_output);
}

float4 PS_GAUAGE(VS_OUT ps_input) : SV_TARGET
{
	float4 vDiffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	float fGauge	= ceil(ps_input.fGauge - ps_input.TexUV.x);
	
	float4 vColor	= vDiffuse * fGauge;
	
	return (vColor);
}