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
float4 PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    float4 Out = 0;
    float2 UV = float2(0, 0);
    for (int j = 0; j < 8; ++j)
    {
        float fTheta = (fc + 2.f * j * 3.14) / 8;
        float x = rA * cos(fTheta) / 800;
        float y = rB * sin(fTheta) / 450;
        float MAX_SAMP = 5;
        float4 m_StarVal[5];
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
    Out /= 8;
    Out.w = 1;
    return Out;
}