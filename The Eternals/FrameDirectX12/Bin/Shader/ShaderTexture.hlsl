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
cbuffer cbMatrixInfo	: register(b0)
{
	float4x4 matWVP		: packoffset(c0);
	float4x4 matWorld	: packoffset(c4);
	float4x4 matView	: packoffset(c8);
	float4x4 matProj	: packoffset(c12);
};

cbuffer cbTexSpriteInfo : register(b1)
{
	int	iFrameCnt		= 1; // 스프라이트 이미지의 X축 개수.
	int iFrameOffset	= 0; // 현재 그려지는 스프라이트의 x축 index.

	int	iSceneCnt		= 1; // 스프라이트 이미지의 Y축 개수.
	int iSceneOffset	= 0; // 현재 그려지는 스프라이트의 y축 index.
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




// VS_TEXTURE_SPRITE
VS_TEX_OUT VS_TEXTURE_SPRITE(VS_TEX_IN vs_input)
{
	VS_TEX_OUT vs_output;
	
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.TexUV	= vs_input.TexUV;
	
	return (vs_output);
}
// PS_TEXTURE_SPRITE
float4 PS_TEXTURE_SPRITE(VS_TEX_OUT ps_input) : SV_TARGET
{
	float u = (ps_input.TexUV.x / (float)iFrameCnt) + iFrameOffset * (1.0f / (float)iFrameCnt);
	float v = (ps_input.TexUV.y / (float)iSceneCnt) + iSceneOffset * (1.0f / (float)iSceneCnt);
	
	float4 Color = g_TexDiffuse.Sample(g_samLinearWrap, float2(u, v));
	
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

