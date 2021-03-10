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
TextureCube g_CubeTex : register(t0);

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

cbuffer cbShaderSkyBox : register(b1)
{
	float4x4	g_matWorld	: packoffset(c0);
}


// VS_MAIN
struct VS_IN
{
	float3 vPos		: POSITION;
	float3 vTexUV	: TEXCOORD0;
};

struct VS_OUT
{
	float4 vPos		: SV_POSITION;
	float3 vTexUV	: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output;

	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	vs_output.vPos		= mul(float4(vs_input.vPos, 1.0f), matWVP);
	vs_output.vTexUV	= vs_input.vTexUV;
	
	return vs_output;
}

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse : SV_TARGET0;	// 0¹ø RenderTarget - Diffuse
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	ps_output.Diffuse = g_CubeTex.Sample(g_samLinearWrap, ps_input.vTexUV);

	return ps_output;
}
