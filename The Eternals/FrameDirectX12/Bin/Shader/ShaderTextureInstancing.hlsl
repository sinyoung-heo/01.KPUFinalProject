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
typedef struct tagShaderTexture
{
	float4x4	matWorld;
	
	// Texture Sprite.
	float		fFrameCnt;	// 스프라이트 이미지 X축 개수.
	float		fCurFrame;	// 현재 그려지는 이미지의 X축 Index.
	float		fSceneCnt;	// 스프라이트 이미지 Y축 개수.
	float		fCurScene;	// 현재 그려지는 이미지의 Y축 Index.
	
	// Texture Gauge.
	float		fGauge;
	float		fOffset1;
	float		fOffset2;
	float		fOffset3;
    float4		vecColor;
    float4		vecOffSet1;
    float4		vecOffSet2;
    float4		vecOffSet3;
    float4		vecOffSet4;
	
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
	
	float	fFrameCnt	: TEXCOORD1;
	float	fCurFrame	: TEXCOORD2;
	float	fSceneCnt	: TEXCOORD3;
	float	fCurScene	: TEXCOORD4;
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
	
	vs_output.fFrameCnt = g_ShaderTexture[iInstanceID].fFrameCnt;
	vs_output.fCurFrame = g_ShaderTexture[iInstanceID].fCurFrame;
	vs_output.fSceneCnt = g_ShaderTexture[iInstanceID].fSceneCnt;
	vs_output.fCurScene = g_ShaderTexture[iInstanceID].fCurScene;
	
	return (vs_output);
}

float4 PS_TEXTURE_SPRITE(VS_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / ps_input.fFrameCnt) + ps_input.fCurFrame * (1.0f / ps_input.fFrameCnt);
	float v = (ps_input.TexUV.y / ps_input.fSceneCnt) + ps_input.fCurScene * (1.0f / ps_input.fSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
   
	return (Color);
}
