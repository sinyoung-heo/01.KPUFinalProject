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
Texture2D g_Tex1024x1024		: register(t0);	// Blend »ö»ó. 1024 1024

Texture2D g_Tex256x256 : register(t1);
Texture2D g_Tex64x64 : register(t2);
Texture2D g_Tex16x16 : register(t3);
Texture2D g_Tex4x4 : register(t4);
Texture2D g_Tex1x1 : register(t5);
/*____________________________________________________________________
[ Vertex Shader ]
______________________________________________________________________*/
struct VS_IN
{
    float3 Pos : POSITION;
    float2 TexUV : TEXCOORD;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 TexUV : TEXCOORD;
};

// VS_MAIN
VS_OUT VS_MAIN(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
	
    vs_output.Pos = float4(vs_input.Pos, 1.0f);
    vs_output.TexUV = vs_input.TexUV;
	
    return vs_output;
}
static const float3 LUM_CONVERT = float3(0.299f, 0.587f, 0.114f);

/*____________________________________________________________________
[ Pixel Shader ]
______________________________________________________________________*/
// PS_MAIN
// PS_MAIN
struct PS_OUT
{
    float4 LUMINANCE : SV_TARGET0; // 0¹ø RenderTarget
};
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT)0;
    float4 Albedo = g_Tex1024x1024.Sample(g_samLinearClamp, ps_input.TexUV);
    output.LUMINANCE = Albedo;
    return (output);
}

PS_OUT PS_LUMINANCE1(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
    float2 vTexUV = ps_input.TexUV;
    float4 vColor = float4(0, 0, 0, 0);

    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 1024.f);
            vTexUV.y = ps_input.TexUV.y + (j / 1024.f);
            vTexUV = saturate(vTexUV);        
            vColor += g_Tex1024x1024.Sample(g_samLinearClamp, vTexUV);
        }
    }
    vColor = vColor / 25;
    output.LUMINANCE = vColor;
    return (output);
}
PS_OUT PS_LUMINANCE2(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
    float2 vTexUV = ps_input.TexUV;
    float4 vColor = float4(0, 0, 0, 0);
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 256.f);
            vTexUV.y = ps_input.TexUV.y + (j / 256.f);
            vTexUV = saturate(vTexUV);
            vColor += g_Tex256x256.Sample(g_samLinearClamp, vTexUV);
        }
    }
    vColor = vColor / 25;
    output.LUMINANCE = vColor;
    return (output);
}
PS_OUT PS_LUMINANCE3(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
    float2 vTexUV = ps_input.TexUV;
    float4 vColor = float4(0, 0, 0, 0);
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 64.f);
            vTexUV.y = ps_input.TexUV.y + (j / 64.f);
            vTexUV = saturate(vTexUV);
            vColor += g_Tex64x64.Sample(g_samLinearClamp, vTexUV);
        }
    }
    vColor = vColor / 25;
    output.LUMINANCE = vColor;
    return (output);
}
PS_OUT PS_LUMINANCE4(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
    float2 vTexUV = ps_input.TexUV;
    float4 vColor = float4(0, 0, 0, 0);
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 16.f);
            vTexUV.y = ps_input.TexUV.y + (j / 16.f);
            vTexUV = saturate(vTexUV);
            vColor += g_Tex16x16.Sample(g_samLinearClamp, vTexUV);
        }
    }
    vColor = vColor / 25;
    output.LUMINANCE = vColor;
    return (output);
}
PS_OUT PS_LUMINANCE5(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
    float2 vTexUV = ps_input.TexUV;
    float4 vColor = float4(0, 0, 0, 0);
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 4.f);
            vTexUV.y = ps_input.TexUV.y + (j / 4.f);
            vTexUV = saturate(vTexUV);
            vColor += g_Tex4x4.Sample(g_samLinearClamp, vTexUV);
        }
    }
    vColor = vColor / 25;
    
    output.LUMINANCE = vColor + float4(0.05, 0.05, 0.05, 0.f);
    output.LUMINANCE.a = 1.f;
    return (output);
}