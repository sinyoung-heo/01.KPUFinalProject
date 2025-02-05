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

float4 PS_MAIN_TOOL(VS_OUT ps_input) : SV_TARGET
{
	float4 ps_output = (float4) 0;
	
	ps_output = g_CubeTex.Sample(g_samLinearWrap, ps_input.vTexUV);
	
	return ps_output;
}

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse	: SV_TARGET0;	// 0번 RenderTarget - Diffuse
    float4 Diffuse1 : SV_TARGET1;	// 0번 RenderTarget - Diffuse
    float4 Diffuse2 : SV_TARGET2;	// 0번 RenderTarget - Diffuse
    float4 Diffuse3 : SV_TARGET3;	// 0번 RenderTarget - Diffuse
    float4 Diffuse4 : SV_TARGET4;	// 0번 RenderTarget - Diffuse
    float4 Diffuse5 : SV_TARGET5;	// 0번 RenderTarget - Diffuse
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
    ps_output.Diffuse5 = g_CubeTex.Sample(g_samLinearWrap, ps_input.vTexUV);
    ps_output.Diffuse.a = 1.f;
	
	return ps_output;
}