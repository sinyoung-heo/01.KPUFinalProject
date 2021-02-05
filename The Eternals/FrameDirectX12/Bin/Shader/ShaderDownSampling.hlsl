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
Texture2D g_TexEmissive		: register(t0);	//Deffered Target Index 4 :  Emissive Target
Texture2D g_TexCrossFilter : register(t1); //CrossFilter Texture
Texture2D g_TexSSAO : register(t2); //SSAO Texture

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

/*____________________________________________________________________
[ Pixel Shader ]
______________________________________________________________________*/
// PS_MAIN
// PS_MAIN
struct PS_OUT
{
    float4 DS_EMISSIVE : SV_TARGET0; // 0¹ø RenderTarget
    float4 DS_CROSSFILTER : SV_TARGET1; // 1¹ø RenderTarget 
    float4 DS_SSAO : SV_TARGET2; // 2¹ø RenderTarget
};
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT)0;
  
    float2 vTexUV = ps_input.TexUV;
    float4 Emissive = float4(0, 0, 0, 0);
    float4 CrossFillter = float4(0, 0, 0, 0); 
    float4 SSAO = float4(0, 0, 0, 0);
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            vTexUV.x = ps_input.TexUV.x + (i / 1600.f);
            vTexUV.y = ps_input.TexUV.y + (j / 900.f);
            vTexUV = saturate(vTexUV);
            Emissive += g_TexEmissive.Sample(g_samLinearClamp, vTexUV);
            SSAO += g_TexSSAO.Sample(g_samLinearClamp, vTexUV);
            
        }
    }
    Emissive /= 25, CrossFillter /= 25, SSAO /= 25;
    output.DS_EMISSIVE = Emissive;
    output.DS_CROSSFILTER = Emissive;
    output.DS_SSAO = SSAO;
    return (output);
}
