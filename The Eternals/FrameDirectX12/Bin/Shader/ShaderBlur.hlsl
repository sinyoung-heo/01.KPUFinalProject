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
Texture2D g_TexEmissive : register(t0); //DownSample Target Index 0 :  Emissive Target  - DownSample
Texture2D g_TexNPathDir : register(t1); ///DownSample Target Index 1  - Cross Fliter Target - DowmSample
Texture2D g_TexDepth : register(t2); //Deffered Target Index 3 :  Depth Target
Texture2D g_TexSSAO : register(t3); //Deffered Target Index 4 :  SSAO Target
Texture2D g_TexEdgeDS : register(t4); //DS Target Index 3 :  Edge Target - DownSample
Texture2D g_TexEdge : register(t5); // Target Index  :  Edge Target
/*
[Static Constant Table]
*/
static float g_fWeight[13] = { 0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216 };
static float g_fOffSet[13] = { -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
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
    float4 EMISSIVE_BLUR        : SV_TARGET0; // 0번 RenderTarget
    float4 CROSSFILTER_BLUR     : SV_TARGET1; // 1번 RenderTarget
    float4 SSAO_BLUR : SV_TARGET2; // 2번 RenderTarget
};
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT)0;
    float2 vGausTexUV = ps_input.TexUV;
   
    float4 Emis_Output = (float4) 0, Emis_Output2 = (float4) 0;
    float4 NPath_Output = (float4) 0, NPath_Output2 = (float4) 0;
    float4 SSAO_Output = (float4) 0, SSAO_Output2 = (float4) 0;
    float4 Edge = g_TexEdge.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Edge_Output = (float4) 0, Edge_Output2 = (float4) 0;
    for (int i = 0; i < 13; i++)
    {
        vGausTexUV.x = ps_input.TexUV.x + (g_fOffSet[i]) / 400.f;
        vGausTexUV = saturate(vGausTexUV);

        Emis_Output += g_TexEmissive.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        NPath_Output += g_TexNPathDir.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        Edge_Output += g_TexEdgeDS.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        vGausTexUV.y = ps_input.TexUV.y + (g_fOffSet[i]) / 225.f;
        vGausTexUV = saturate(vGausTexUV);
     
        Emis_Output2 += g_TexEmissive.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        NPath_Output2 += g_TexNPathDir.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        Edge_Output2 += g_TexEdgeDS.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        
        vGausTexUV.x = ps_input.TexUV.x + (g_fOffSet[i] /12) / 200.f;
        SSAO_Output += g_TexSSAO.Sample(g_samAnisotropicClamp, vGausTexUV) * (g_fWeight[i]);
        
        vGausTexUV.y = ps_input.TexUV.y + (g_fOffSet[i]/12) / 112.5f;
        SSAO_Output2 += g_TexSSAO.Sample(g_samAnisotropicClamp, vGausTexUV) * (g_fWeight[i]);
     
    }
    float ouputEdge = (Edge_Output + Edge_Output2)*0.5f;
    output.EMISSIVE_BLUR = (Emis_Output + Emis_Output2) *0.5f;
    output.CROSSFILTER_BLUR = (NPath_Output + NPath_Output2) * 0.5f;
  
    output.SSAO_BLUR = (SSAO_Output + SSAO_Output2);
    
    output.EMISSIVE_BLUR += output.CROSSFILTER_BLUR;//최종때 이미시브에 더할것들은 합산해주어 블렌드에서 따로 안받도록
    output.EMISSIVE_BLUR += (ouputEdge + Edge*2.f)*0.5f;
    return (output);
}
