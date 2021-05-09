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
Texture2D Effect0 : register(t0);	//
Texture2D Effect1 : register(t1);
Texture2D Effect2 : register(t2);
Texture2D Effect3 : register(t3);
Texture2D Effect4 : register(t4);
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

float4 PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    float4 output0 = Effect0.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 output1 = Effect1.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 output2 = Effect2.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 output3 = Effect3.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 output4 = Effect4.Sample(g_samLinearWrap, ps_input.TexUV);

    float4 FinalOut = output0 + output1 + output2 + output3 + output4;
    FinalOut.a = 1;
    return FinalOut;
}
