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
Texture2D g_TexDiffuse		: register(t0);	// Diffuse 색상.
Texture2D g_TexNormal		: register(t1);	// 탄젠트 공간 Normal Map.
Texture2D g_TexSpecular		: register(t2);	// Specular 강도.
Texture2D g_TexShadowDepth	: register(t3);	// ShadowDepth
Texture2D g_TexDissolve		: register(t4); // Dissolve
/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView		: packoffset(c0);
	float4x4	g_matProj		: packoffset(c4);
	float4		g_vCameraPos	: packoffset(c8);
	float		g_fProjFar		: packoffset(c9.x);
	
	float		g_fCMOffset1	: packoffset(c9.y);
	float		g_fCMOffset2	: packoffset(c9.z);
	float		g_fCMOffset3	: packoffset(c9.w);
}

cbuffer cbShaderMesh : register(b1)
{
    float4x4 g_matWorld : packoffset(c0);
    float4x4 g_matLightView : packoffset(c4);
    float4x4 g_matLightProj : packoffset(c8);
    float4 g_vLightPos : packoffset(c12);
    float g_fLightPorjFar : packoffset(c13.x);
    float g_fDissolve : packoffset(c13.y);
    float g_fOffset1 : packoffset(c13.z);
    float g_fOffset2 : packoffset(c13.w);
    float g_fOffset3 : packoffset(c14.x);
    float g_fOffset4 : packoffset(c14.y);
    float g_fOffset5 : packoffset(c14.z);
    float g_fOffset6 : packoffset(c14.w);
    float4 g_vAfterImgColor : packoffset(c15);
    float4 g_vEmissiveColor : packoffset(c16);
    float4 g_vTexPos : packoffset(c17);
};


// VS_MAIN
struct VS_IN
{
	float3 Pos				: POSITION;
	float3 Normal			: NORMAL;
	float2 TexUV			: TEXCOORD;
	float4 BoneId[2]		: TEXCOORD1;
	float4 BoneWeights[2]	: TEXCOORD3;
};

struct VS_OUT
{
	float4 Pos			: SV_POSITION;
	float2 TexUV		: TEXCOORD0;
	float3 Normal		: TEXCOORD1;
	
	float3 T			: TEXCOORD2;
	float3 B			: TEXCOORD3;
	float3 N			: TEXCOORD4;
	
	float4 ProjPos		: TEXCOORD5;
	float4 LightPos		: TEXCOORD6;
    float4 WorldPos		: TEXCOORD7;
    float2 AniUV		: TEXCOORD8;
    
    float fDissolve : TEXCOORD9;
    float fOffset1 : TEXCOORD10;
    float fOffset2 : TEXCOORD11;
    float fOffset3 : TEXCOORD12;
};

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse		: SV_TARGET0;	// 0번 RenderTarget - Diffuse
	float4 Normal		: SV_TARGET1;	// 1번 RenderTarget - Normal
	float4 Specular		: SV_TARGET2;	// 2번 RenderTarget - Specular
	float4 Depth		: SV_TARGET3;	// 3번 RenderTarget - Depth
    float4 Emissive		: SV_TARGET4;	// 4번 RenderTarget - Emissive
};

/*__________________________________________________________________________________________________________
[ TerrainMesh  ]
____________________________________________________________________________________________________________*/
static float fDetails = 20.0f;

VS_OUT VS_TERRAIN_MAIN(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;	
    float4x4 matWV, matWVP;
    matWV = mul(g_matWorld, g_matView);
    matWVP = mul(matWV, g_matProj);	
    
    vs_output.WorldPos = mul(float4(vs_input.Pos, 1.f), g_matWorld);
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;
	
	// N
    float3 WorldNormal = mul(vs_input.Normal, (float3x3)g_matWorld);
    vs_output.N = normalize(WorldNormal);
	
	// T
    float3 Tangent = cross(float3(0.f, 1.f, 0.f), (float3) vs_input.Normal);
    float3 WorldTangent = mul(Tangent, (float3x3) g_matWorld);
    vs_output.T = normalize(WorldTangent);
	
	// B
    float3 Binormal = cross((float3) vs_input.Normal, Tangent);
    float3 WorldBinormal = mul(Binormal, (float3x3) g_matWorld);
    vs_output.B = normalize(WorldBinormal);
	
	// ProjPos
    vs_output.ProjPos = vs_output.Pos;
	
	// 광원 위치에서의 투영 좌표
    float4 matLightPosW = mul(float4(vs_input.Pos.xyz, 1.0f), g_matWorld);
    float4 matLightPosWV = mul(float4(matLightPosW.xyz, 1.0f), g_matLightView);
    vs_output.LightPos = mul(float4(matLightPosWV.xyz, 1.0f), g_matLightProj);
    vs_output.LightPos.z = vs_output.LightPos.z * vs_output.LightPos.w / g_fLightPorjFar;
	 // Dissolve
    vs_output.fDissolve =g_fDissolve;
    vs_output.fOffset1 =g_fOffset1;
    vs_output.fOffset2 =g_fOffset2;
    
    vs_output.fOffset3 = g_fOffset3;//Radius
    return (vs_output);
}

static float WAVE = 0.01f; //0.004f;
PS_OUT PS_TERRAIN_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    clip(ps_input.TexUV.y + 0.1f);
    float clipSpace = (ps_input.TexUV.y) - 0.1f + cos(ps_input.fOffset2 * 0.5f) * 0.1f - sin(ps_input.TexUV.x * 50.f) * (ps_input.fDissolve * 0.01f);

	// Diffuse
    ps_output.Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV * fDetails);
	
    if (clipSpace < 0.f)
        ps_output.Diffuse.rgb *= 0.8f;
	// Normal
    float4 TexNormal = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV * fDetails);
    TexNormal = (TexNormal * 2.0f) - 1.0f; // 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
    float3 Normal = (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
    ps_output.Normal = float4(Normal.xyz * 0.5f + 0.5f, 1.f); // 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	
	
	
	// Depth
    ps_output.Depth = float4(ps_input.ProjPos.z / ps_input.ProjPos.w, // (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar, // posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 1.0f, 1.0f);
	
	/*__________________________________________________________________________________________________________
	[ 현재의 깊이와 그림자 깊이 비교 ]
	____________________________________________________________________________________________________________*/
    float2 uv = ps_input.LightPos.xy / ps_input.LightPos.w;
    uv.y = uv.y * -0.5f + 0.5f;
    uv.x = uv.x * 0.5f + 0.5f;
	
    float CurrentDepth = ps_input.LightPos.z / ps_input.LightPos.w;
    float ShadowDepth = g_TexShadowDepth.Sample(g_samLinearWrap, uv).x;
    if (CurrentDepth > ShadowDepth + 0.0000125f)
        ps_output.Diffuse.rgb *= 0.5;
	
    return (ps_output);
}

PS_OUT PS_MAGICCIRCLE(VS_OUT ps_input) : SV_TARGET
{
   // Diffuse
    PS_OUT ps_output = (PS_OUT) 0;

    ps_output.Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV );
    
    // Depth
    ps_output.Depth = float4(ps_input.ProjPos.z / ps_input.ProjPos.w, // (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar, // posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 1.0f, 1.0f);
   // ps_output.Diffuse = mul(ps_output.Diffuse.rgb, ps_output.Diffuse.a);
    return (ps_output);
}