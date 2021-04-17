/*__________________________________________________________________________________________________________
[ Shader Resource ]
____________________________________________________________________________________________________________*/
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
	float4 ProjPos	: TEXCOORD0;
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
	
	// ShadowDepth - 광원 위치에서의 깊이 값 기록.
	float Depth = ps_input.ProjPos.z / ps_input.ProjPos.w; // (posWVP.z / posWVP.w) : Proj 영역의 Z.
	
	ps_output.ShadowDepth = float4(Depth.x, 0.0f, 0.0f, 1.0f);
	
	return (ps_output);
}
