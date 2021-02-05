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
Texture2D g_TexNormal	: register(t0);	 //Deffered Target Index 1 :  Normal Target
Texture2D g_TexDepth    : register(t1);  //Deffered Target Index 3 :   Depth Target

//CBuffer
/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
    float4x4 g_matView : packoffset(c0);
    float4x4 g_matProj : packoffset(c4);
    float4 g_vCameraPos : packoffset(c8);
    float g_fProjFar   : packoffset(c9.x);
}

////global Constant 
//static float3 vSamples[16] =
//{
//    float3(0.355512, -0.709318, -0.102371), float3(0.534186, 0.71511, -0.115167),float3(-0.87866, 0.157139, -0.115167), float3(0.140679, -0.475516, -0.0639818),
//	float3(-0.0796121, 0.158842, -0.677075),float3(-0.0759516, -0.101676, -0.483625),float3(0.12493, -0.0223423, -0.483625),float3(-0.0720074, 0.243395, -0.967251),
//	float3(-0.207641, 0.414286, 0.187755),float3(-0.277332, -0.371262, 0.187755),float3(0.63864, -0.114214, 0.262857), float3(-0.184051, 0.622119, 0.262857),
//	float3(0.110007, -0.219486, 0.435574),float3(0.235085, 0.314707, 0.696918),float3(-0.290012, 0.0518654, 0.522688),float3(0.0975089, -0.329594, 0.609803)
//};

float3 randomNormal(float2 tex)
{
    float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f));
    float noiseY = (frac(sin(dot(tex, float2(11.9898f, 62.223f) * 2.0f)) * 34748.34744f));
    float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f));
    return normalize(float3(noiseX, noiseY, noiseZ));
}
float4 Get_SSAO(float2 Uv, float Depth, float ViewZ, half3 Normal)
{
    float4 Out = (float4) 0.f;
    half3 vRay,vReflect;
    half2 vRandomUV;
    float fOccNorm;
    int iColor = 0;
    float g_fRadius = 0.001f;

    float3 vSamples[16] =
    {
        float3(0.355512, -0.709318, -0.102371),
	    float3(0.534186, 0.71511, -0.115167),
	    float3(-0.87866, 0.157139, -0.115167),
	    float3(0.140679, -0.475516, -0.0639818),
	    float3(-0.0796121, 0.158842, -0.677075),
	    float3(-0.0759516, -0.101676, -0.483625),
	    float3(0.12493, -0.0223423, -0.483625),
	    float3(-0.0720074, 0.243395, -0.967251),
	    float3(-0.207641, 0.414286, 0.187755),
	    float3(-0.277332, -0.371262, 0.187755),
	    float3(0.63864, -0.114214, 0.262857),
	    float3(-0.184051, 0.622119, 0.262857),
	    float3(0.110007, -0.219486, 0.435574),
	    float3(0.235085, 0.314707, 0.696918),
	    float3(-0.290012, 0.0518654, 0.522688),
	    float3(0.0975089, -0.329594, 0.609803)
    };
    for (int i = 0; i < 16; i++)
    {
        vRay = reflect(randomNormal(Uv), vSamples[i]);
        vReflect = normalize(reflect(vRay, Normal)) * g_fRadius;
        vReflect.x *= -1.f;
        vRandomUV = Uv + vReflect.xy;
        fOccNorm = g_TexDepth.Sample(g_samLinearWrap, vRandomUV).r * ViewZ;
        
        
        if (fOccNorm <= Depth + 0.0003)
            ++iColor;

    }
    Out = abs((iColor / 16.f) - 1.f);
    return Out;
}
 
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
    float4 SSAO : SV_TARGET0; // 0¹ø RenderTarget
};


PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT) 0;
  
    float4 vDepth = g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 vNormal = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    vNormal = normalize(vNormal * 2.f - 1.f);
    float2 vUv = ps_input.TexUV;
    float fViewZ = vDepth.g * 1000.f;
    float fDepth = vDepth.r * fViewZ;
    float3 Normal = vNormal.rgb;
    float4 Out = Get_SSAO(vUv, fDepth, fViewZ, Normal);
    float3 Out2 = 1 - Out.xyz;
    
    output.SSAO = float4(Out2, 1);
    
    return output;
    //float4 vWorldPos = vector(ps_input.TexUV.x * 2.f - 1.f, -ps_input.TexUV.y * 2 + 1, Depth.r, 1.f);
    
    //vWorldPos = mul(vWorldPos, g_matProj);//Inv
    //vWorldPos = mul(vWorldPos, g_matView);//Inv
    
    //vWorldPos = vWorldPos / vWorldPos.w;
    
    
    //float4 Normal = g_TexNormal.Sample(g_samLinearClamp, ps_input.TexUV);
    //float Color = Depth.r * (Normal.g) * 0.7f;
    //output.SSAO = float4(Color.rrr, 1.f);
}
