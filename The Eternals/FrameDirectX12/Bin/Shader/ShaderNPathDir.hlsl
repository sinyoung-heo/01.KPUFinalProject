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
Texture2D g_TexEmissive	  : register(t0);	// Differed - Emisive
Texture2D g_TexCrossFilterObject : register(t1); // g_TexCrossFilterObject
Texture2D g_TexDepth : register(t2); // g_TexDepth
Texture2D g_TexCrossFilterDepth : register(t3); //
Texture2D g_TexSkyBox : register(t4); //
Texture2D g_TexSkyBoxBright : register(t5); //
cbuffer cbShaderVariable : register(b0)
{
    float4 g_float4 : packoffset(c0);
}

/*____________________________________________________________________
[ Vertex Shader ]
______________________________________________________________________*/
struct VS_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_OUT
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
};


// VS_MAIN
VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	vs_output.Pos	= float4(vs_input.Pos, 1.0f);
	vs_output.TexUV = vs_input.TexUV;
	
	return vs_output;
}

//global Constant 
static float rA = 7; //4.5f; //X
static float rB = 9; //6.5f; //Y
static float fc = 0.2f;
/*____________________________________________________________________
[ Pixel Shader ]
______________________________________________________________________*/
// PS_MAIN
#define PATH 8
#define MAX_SAMP 6

float4 PS_HIGHLIGHT(VS_OUT ps_input) : SV_TARGET
{
    float4 SkyBox = g_TexSkyBox.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Depth = g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float Offset = SkyBox.r + SkyBox.g + SkyBox.b;
    if(Offset>2.9999f)
    {
        SkyBox.rgb = pow(SkyBox.rgb, 2.2f);
        SkyBox = float4(1, 1, 1, 1);
        SkyBox *= (1 - Depth.b);
        return SkyBox;
    }
    return float4(0, 0, 0, 0);
}

float4 PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    float4 Out = 0,Out2=0,Out3=0;
    float2 UV = float2(0, 0);
    for (int j = 0; j < PATH; ++j)
    {
        float fTheta = (g_float4.x + 2.f * j * 3.14) / PATH;
        float x = rA * cos(fTheta) / 800;
        float y = rB * sin(fTheta) / 450;
        float4 m_StarVal[MAX_SAMP];
        for (int i = 0; i < MAX_SAMP; ++i)
        {
            m_StarVal[i].x = x * i;
            m_StarVal[i].y = y * i;
            m_StarVal[i].z = exp(-i * i / 10);
            UV.x = ps_input.TexUV.x + m_StarVal[i].x * 1.25f;
            UV.y = ps_input.TexUV.y + m_StarVal[i].y * 1.25f;
            UV = saturate(UV);
            Out += g_TexEmissive.Sample(g_samLinearClamp, UV); // * m_StarVal[i].z;

        }
    }
    
    float4 Depth = g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 CrossFilterDepth = g_TexCrossFilterDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    int Cnt = 1;
    
   
        for (j = 10; j < 20; ++j)
        {
            float fTheta = ( /*g_float4.x*/2.f * j * 3.14) / 20;
            float x = rA * cos(fTheta) / 800;
            float y = rB * sin(fTheta) / 450;
            float4 m_StarVal[40];
            for (int i = 0; i < 40; ++i)
            {
                Cnt++;
                m_StarVal[i].x = x * i;
                m_StarVal[i].y = y * i;
                m_StarVal[i].z = exp(-i * i / 10);
                UV.x = ps_input.TexUV.x + m_StarVal[i].x * 1.25f;
                UV.y = ps_input.TexUV.y + m_StarVal[i].y * 1.25f;
                UV = saturate(UV);
                Out2 += g_TexSkyBoxBright.Sample(g_samLinearClamp, UV);
            }
        }

    Out /= PATH, Out.w = 1;
    Out2 /= (Cnt *0.5),Out2.w = 1;
    
    Out3 = g_TexCrossFilterObject.Sample(g_samLinearClamp,ps_input.TexUV);
    return Out + Out2 + Out3;
}

  //for (j = 0; j < 8; ++j) {
    //    float fTheta = (fc + 2.f * j * 3.14) / 8;
    //    float x = rA * cos(fTheta) / 800;
    //    float y = rB * sin(fTheta) / 450;
    //    float4 m_StarVal[5];
    //    for (int i = 0; i < 5; ++i)  {
    //        m_StarVal[i].x = x * i;
    //        m_StarVal[i].y = y * i;
    //        m_StarVal[i].z = exp(-i * i / 10);
    //        UV.x = ps_input.TexUV.x + m_StarVal[i].x * 1.25f;
    //        UV.y = ps_input.TexUV.y + m_StarVal[i].y * 1.25f;
    //        UV = saturate(UV);
    //        Out2 += g_TexCrossFilterObject.Sample(g_samLinearClamp, UV);
    //    }
    //}