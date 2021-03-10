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
Texture2D g_TexEdgeObject		: register(t0);	//
Texture2D g_TexDepth : register(t1);
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
    float4 EDGE : SV_TARGET0; // 0¹ø RenderTarget
};
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT)0;
    float2 TexUV = float2(0,0);
    float4 Depth = g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float ViewZ = Depth.g;
    int Cnt = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            TexUV = float2(ps_input.TexUV.x + (j / (400.f + (ViewZ * 160.f))),
            ps_input.TexUV.y + (i / (225.f + (ViewZ * 900.f))));
            if (g_TexEdgeObject.Sample(g_samLinearWrap, TexUV).a > 0.01)
                Cnt++;
        }
    }  
    if (Cnt >= 1 && Cnt != 9)
        output.EDGE = float4(0.3f, 0.6f, 1.f, 1);

    
    return (output);
}
