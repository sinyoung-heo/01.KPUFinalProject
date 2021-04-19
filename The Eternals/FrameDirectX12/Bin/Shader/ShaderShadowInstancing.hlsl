/*__________________________________________________________________________________________________________
[ Shader Resource ]
____________________________________________________________________________________________________________*/

/*__________________________________________________________________________________________________________
[ Sampler ]
____________________________________________________________________________________________________________*/
SamplerState g_samPointWrap : register(s0);
SamplerState g_samPointClamp : register(s1);
SamplerState g_samLinearWrap : register(s2);
SamplerState g_samLinearClamp : register(s3);
SamplerState g_samAnisotropicWrap : register(s4);
SamplerState g_samAnisotropicClamp : register(s5);


typedef struct tagShaderMesh
{
	float4x4	matWorld;				
	float4x4	matView;
	float4x4	matProj;
	float		fProjFar;
	float		fOffset1;
	float		fOffset2;
	float		fOffset3;
	
} SHADER_SHADOW;
StructuredBuffer<SHADER_SHADOW> g_ShaderShadow : register(t0, space1);

/*__________________________________________________________________________________________________________
[ Texture ]
____________________________________________________________________________________________________________*/
Texture2D g_TexDiffuse : register(t1); // Diffuse 색상.

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
	float4 Pos		: SV_POSITION;
    float2 TexUV    : TEXCOORD0;
	float4 ProjPos	: TEXCOORD1;
    float AlphaTest : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{	
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV			= mul(g_ShaderShadow[iInstanceID].matWorld, g_ShaderShadow[iInstanceID].matView);
	matWVP			= mul(matWV, g_ShaderShadow[iInstanceID].matProj);
	vs_output.Pos	= mul(float4(vs_input.Pos, 1.0f), matWVP);
	
	// 광원 위치에서의 투영 좌표 계산.
	float4 vPos			= vs_output.Pos;
	vPos.z				= vPos.z * vPos.w / g_ShaderShadow[iInstanceID].fProjFar;
	vs_output.ProjPos	= vPos;
	
    vs_output.TexUV = vs_input.TexUV;
	
    vs_output.AlphaTest = g_ShaderShadow[iInstanceID].fOffset1;
	return (vs_output);
}


// PS_MAIN
struct PS_OUT
{
	float4 ShadowDepth : SV_TARGET0;	 // 0번 RenderTarget - ShadowDepth
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
    float4 Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	
	// ShadowDepth - 광원 위치에서의 깊이 값 기록.
	float Depth = ps_input.ProjPos.z / ps_input.ProjPos.w; // (posWVP.z / posWVP.w) : Proj 영역의 Z.
	
    if (ps_input.AlphaTest) {
        ps_output.ShadowDepth = Diffuse; //Diffuse;
        ps_output.ShadowDepth.x = Depth;
    }
	else
        ps_output.ShadowDepth = float4(Diffuse.x, 0.f, 0.f, 1.f);
	return (ps_output);
}
