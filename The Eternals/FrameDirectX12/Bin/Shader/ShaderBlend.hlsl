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
Texture2D g_TexDiffuse	  : register(t0);	// Albedo -> DOF
Texture2D g_TexShade	  : register(t1);	// Shade
Texture2D g_TexSpecular   : register(t2);	// Specular
Texture2D g_TexBlur		  : register(t3); // Blur_Emissive
Texture2D g_TexEmissive   : register(t4); // Emissive
Texture2D g_TexSSAO		  : register(t5); // SSAO
Texture2D g_TexDistortion : register(t6); // Distortion
Texture2D g_TexAverageColor : register(t7); // AveColor
Texture2D g_TexBlend : register(t8); // Blend
Texture2D g_TexDepth : register(t9); //Depth
Texture2D g_TexEdgeBlur : register(t10); //Edge-Blur
Texture2D g_TexSkyBox : register(t11); //SB

Texture2D g_TexEffect : register(t12); //EFF
cbuffer IS_Rendering : register(b0)
{
    float fSsao            : packoffset(c0.x);
    float fDepthOfField    : packoffset(c0.y);
    float fShade           : packoffset(c0.z);
    float fToneMapping   : packoffset(c0.w);
    float fDistortion      : packoffset(c1.x);
    float fSpecular         : packoffset(c1.y);
    float fOffset2         : packoffset(c1.z);
    float fOffset3         : packoffset(c1.w);
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

static const float3 LUM_CONVERT = float3(0.299f, 0.587f, 0.114f);

float3 ToneMapping(float3 vHDRColor,float3 AveLumi)
{
    float g_fMiddleGrey = 0.8f;
    float g_fLumWhiteSqr = 5.7f;
    // 현재 픽셀에 대한 휘도 스케일 계산
    float fLScale =dot(vHDRColor, LUM_CONVERT);
    fLScale *= (g_fMiddleGrey / AveLumi.r);
    fLScale = (fLScale + fLScale * fLScale / g_fLumWhiteSqr) / (1.f + fLScale);

    // 휘도 스케일을 픽셀 색상에 적용
    return vHDRColor * fLScale;
}

// VS_MAIN
VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	vs_output.Pos	= float4(vs_input.Pos, 1.0f);
	vs_output.TexUV = vs_input.TexUV;
	
	return vs_output;
}

/*____________________________________________________________________
[ Pixel Shader ]
______________________________________________________________________*/
// PS_MAIN
float4 PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    //float4 Distortion = g_TexDistortion.Sample(g_samLinearWrap, ps_input.TexUV);
    
    //float2 Normal = Distortion.xy;
    //Normal = Normal * 2.0f - 1.0f;
    //float2 TexUV = (Normal.xy ) + ps_input.TexUV;
    vector vDistortionInfo = g_TexDistortion.Sample(g_samLinearWrap, ps_input.TexUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= 0.05f;//강도
    float2 TexUV = float2(ps_input.TexUV.x + vDistortion.x * vDistortionInfo.b, ps_input.TexUV.y + vDistortion.y * vDistortionInfo.b);
    TexUV = saturate(TexUV);
	float4 Albedo	= g_TexDiffuse.Sample(g_samLinearWrap, TexUV);
	float4 Shade	= g_TexShade.Sample(g_samLinearWrap, TexUV)*1.5f;
	float4 Specular = g_TexSpecular.Sample(g_samLinearWrap, TexUV);
    float4 Emissive = g_TexEmissive.Sample(g_samLinearWrap, TexUV);
    float4 SSAO = g_TexSSAO.Sample(g_samLinearWrap, TexUV);
    
    //On Off
    if (fShade) Shade.xyz = fShade;
    if (fSsao) SSAO.xyz = fSsao;
    Specular *= fSpecular;
    float4 Color = Albedo * (Shade * SSAO) + Specular;// +Blur + Emissive;
    
    Color.a = Albedo.a;
    return Color;
}
/*
[Static ConstantTable]
*/
static float g_fWeight[13] = { 0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216 };
static float g_fOffSet[13] = { -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };

//DOF수치 Depth에 따라
//1600~800 max 800
//900~450 max 450

//ViewZ값에서 50 이상 떨어진 부분만 DOF 처리
float4 PS_FINAL(VS_OUT ps_input) : SV_TARGET
{
    float4 AverageColor = g_TexAverageColor.Sample(g_samLinearWrap, float2(0.5f, 0.5f));
    float4 BlendTarget = g_TexBlend.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Blur = g_TexBlur.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 EdgeBlur = g_TexEdgeBlur.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Emissive = g_TexEmissive.Sample(g_samLinearWrap, ps_input.TexUV);
    float3 Color;
    float4 Output = float4(0, 0, 0, 0), Output2 = float4(0, 0, 0, 0);
    float4 Depth = g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float ViewZ = Depth.g;
    //0 ~1값의 ViewZ 값 멀어질수록 1에 가까워진다. 거리 0.05이상부터 DOF 처리를 매길것임
    //50 : 1000 = 1600 : 650
    //50 : 1000 = 900 : 
  
    float4 SkyBox = g_TexSkyBox.Sample(g_samLinearWrap, ps_input.TexUV) * (1 - Depth.b);
   
    float2 vGausTexUV = float2(0, 0);
    if (ViewZ > 0.05f && fDepthOfField){ for (int i = 0; i < 13; i++){
            vGausTexUV.x = ps_input.TexUV.x + (g_fOffSet[i]) / (3200.f - ViewZ *2000.f );
            vGausTexUV = saturate(vGausTexUV);
            Output += g_TexBlend.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
     
            vGausTexUV.y = ps_input.TexUV.y + (g_fOffSet[i]) / (1800.f - (ViewZ * 1068.3f));
            vGausTexUV = saturate(vGausTexUV);
            Output2 += g_TexBlend.Sample(g_samLinearWrap, vGausTexUV) * (g_fWeight[i]);
        }}
    else
        Output = Output2 = g_TexBlend.Sample(g_samLinearWrap, ps_input.TexUV);
    BlendTarget = (Output + Output2) * 0.5f;

    //OnOff
    if(!fToneMapping)
        Color = ToneMapping(BlendTarget.xyz, AverageColor.xyz) + mul(Blur.xyz, 2.f) + Emissive.xyz;
    else
        Color = BlendTarget.xyz + mul(Blur.xyz, 2.f) + Emissive.xyz;

    Color += (EdgeBlur + SkyBox);
    
    Color += g_TexEffect.Sample(g_samLinearWrap, ps_input.TexUV);
    return float4(Color, BlendTarget.a);
    
    
    //return BlendTarget;
    }
