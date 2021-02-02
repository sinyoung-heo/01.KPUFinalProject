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
Texture2D g_TexDiffuse		: register(t0);	// Diffuse »ö»ó.

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
    float4 LUMINANCE : SV_TARGET0; // 0¹ø RenderTarget
};
PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT output = (PS_OUT)0;
    float4 Albedo = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
  
    Albedo.rgb *= 2.f;
    if (Albedo.r + Albedo.b + Albedo.b < 2.f)
        Albedo.rgb = 0.f;

    Albedo = smoothstep(0.6, 0.9, Albedo);
    
    output.LUMINANCE = Albedo;
	
    
    
    return (output);
}
