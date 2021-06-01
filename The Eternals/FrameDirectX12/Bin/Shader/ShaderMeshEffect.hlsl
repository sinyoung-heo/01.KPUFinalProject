/*__________________________________________________________________________________________________________
[ Sampler ]
____________________________________________________________________________________________________________*/
SamplerState g_samPointWrap			: register(s0);
SamplerState g_samPointClamp		: register(s1);
SamplerState g_samLinearWrap		: register(s2);
SamplerState g_samLinearClamp		: register(s3);
SamplerState g_samAnisotropicWrap	: register(s4);
SamplerState g_samAnisotropicClamp	: register(s5);


/*__________________________________________________________________________________________________________
[ Texture ]
____________________________________________________________________________________________________________*/
Texture2D g_TexDiffuse : register(t0); // Diffuse 색상.
Texture2D g_TexNormal		: register(t1);	// 탄젠트 공간 Normal Map.
Texture2D g_TexSpecular		: register(t2);	// Specular 강도.
Texture2D g_TexShadowDepth	: register(t3);	// ShadowDepth
Texture2D g_TexDissolve		: register(t4); // Dissolve
/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView		: packoffset(c0);
	float4x4	g_matProj		: packoffset(c4);
	float4		g_vCameraPos	: packoffset(c8);
	float		g_fProjFar		: packoffset(c9.x);
	
	float		g_fCMOffset1	: packoffset(c9.y);
	float		g_fCMOffset2	: packoffset(c9.z);
	float		g_fCMOffset3	: packoffset(c9.w);
}
cbuffer cbShaderMesh : register(b1)
{
	float4x4	g_matWorld			: packoffset(c0);
	float4x4	g_matLightView		: packoffset(c4);
	float4x4	g_matLightProj		: packoffset(c8);
	float4		g_vLightPos			: packoffset(c12);
	float		g_fLightPorjFar		: packoffset(c13.x);
    float		g_fDissolve			: packoffset(c13.y);
	float		g_fOffset1			: packoffset(c13.z);
	float		g_fOffset2			: packoffset(c13.w);
    float		g_fOffset3			: packoffset(c14.x);
    float		g_fOffset4			: packoffset(c14.y);
    float		g_fOffset5			: packoffset(c14.z);
    float		g_fOffset6			: packoffset(c14.w);
	float4		g_vAfterImgColor	: packoffset(c15);
	float4		g_vEmissiveColor	: packoffset(c16);
};
struct VS_IN
{
	float3 Pos				: POSITION;
	float3 Normal			: NORMAL;
	float2 TexUV			: TEXCOORD;
};

struct VS_OUT
{
	float4 Pos			: SV_POSITION;
	float2 TexUV		: TEXCOORD0;
	float3 Normal		: TEXCOORD1;
	float3 T			: TEXCOORD2;
	float3 B			: TEXCOORD3;
	float3 N			: TEXCOORD4;
	float4 ProjPos		: TEXCOORD5;
	float4 LightPos		: TEXCOORD6;
    float4 WorldPos		: TEXCOORD7;
    float2 AniUV		: TEXCOORD8;
};

// PS_MAIN
struct PS_OUT
{
    float4 Effect0 : SV_TARGET0; // 
    float4 Effect1 : SV_TARGET1; // 
    float4 Effect2 : SV_TARGET2; // 
    float4 Effect3 : SV_TARGET3; // 
    float4 Effect4 : SV_TARGET4; // 
};

/*__________________________________________________________________________________________________________
[ 그림자 (X) ]
____________________________________________________________________________________________________________*/
VS_OUT VS_MAIN(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
    float4x4 matworld = g_matWorld;
    float4x4 matWV, matWVP;
    matWV = mul(matworld, g_matView);
    matWVP = mul(matWV, g_matProj);	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;
    return (vs_output);
}
VS_OUT VS_ANIUV(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
    float4x4 matworld = g_matWorld;
    float4x4 matWV, matWVP;
    matWV = mul(matworld, g_matView);
    matWVP = mul(matWV, g_matProj);
	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;

    
    vs_output.AniUV = vs_input.TexUV + float2(g_fOffset1, -g_fOffset1);
    return (vs_output);
}

PS_OUT PS_MAGIC_CIRCLE(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    float4 Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 TexNormal = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Spec = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = lerp(lerp(Diffuse, Spec, 0.5), TexNormal, 0.5);
    ps_output.Effect2 = color;
  
    return ps_output;
}

PS_OUT PS_RAINDROP(VS_OUT ps_input) : SV_TARGET
{
   
    PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
    ps_output.Effect4 = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    //ps_output.Effect4 = (g_TexDiffuse.Sample(g_samLinearWrap, ps_input.AniUV * 10.f));
    ps_output.Effect4.a = 1.f;
    return (ps_output);
}

PS_OUT PS_EFFECT_SHPERE(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    //float u = (ps_input.TexUV.x / g_vEmissiveColor.x) + g_vEmissiveColor.z * (1.0f / g_vEmissiveColor.x);
    //float v = (ps_input.TexUV.y / g_vEmissiveColor.y) + g_vEmissiveColor.w * (1.0f / g_vEmissiveColor.y);
   // float2 TexUV = float2((u + g_fOffset1*0.002f),( v + g_fOffset1*0.0005f));
   // TexUV.x *= 2.f;
    float2 TexUV = float2(ps_input.TexUV.x, ps_input.TexUV.y);
    clip(ps_input.TexUV.y + g_fOffset1);
    ps_output.Effect1 = g_TexDiffuse.Sample(g_samLinearWrap, TexUV);
    return (ps_output);
}
PS_OUT PS_ICESTORM(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 E = float4(0, 0, 0, 1);
    float Normal_fDissolve = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV).r;

    if ((0.05f > (1.f - g_fDissolve) - Normal_fDissolve) && ((1.f - g_fDissolve) - Normal_fDissolve) > 0.f)
    {
        E = N;
    }
    if ((0.05f > (1.f - g_fOffset1) - Normal_fDissolve) && ((1.f - g_fOffset1) - Normal_fDissolve) > 0.f)
    {
        E += N;
    }
    clip((1.f - g_fDissolve) - Normal_fDissolve);
    
    ps_output.Effect1 = lerp((D + N), E, 0.5f);
    ps_output.Effect1.a = 0.5f;
    return (ps_output);
}
PS_OUT PS_DECAL(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= g_fOffset1; //강도
    float2 NewUV = float2(ps_input.TexUV.x + vDistortion.x * vDistortionInfo.b, ps_input.TexUV.y + vDistortion.y * vDistortionInfo.b);
   
    float2 AniUV = ps_input.AniUV;
   
    
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, NewUV *2);
  
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    
    float4 color = mul(D.r, N) + mul(D.g, float4(0.6 + g_fOffset2, g_fOffset2, 0, 1)) + mul(D.b, float4(1.f, 0.5, 0.5, 1));
 
	
    ps_output.Effect4 = color;
    ps_output.Effect4.a = 1;
    return (ps_output);
}
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
   
    ps_output.Effect3 = D;
    return (ps_output);
}
