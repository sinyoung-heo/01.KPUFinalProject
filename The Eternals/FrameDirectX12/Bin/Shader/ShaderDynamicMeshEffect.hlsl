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
	float4x4	g_matWorld			: packoffset(c0);
	float4x4	g_matLightView		: packoffset(c4);
	float4x4	g_matLightProj		: packoffset(c8);
	float4		g_vLightPos			: packoffset(c12);
	float		g_fLightPorjFar		: packoffset(c13.x);
    float		g_fDissolve			: packoffset(c13.y);
	float		g_fOffset1			: packoffset(c13.z);
	float		g_fOffset2			: packoffset(c13.w);
    float		g_fOffset3			: packoffset(c14.x);
    float		g_fOffset4			: packoffset(c14.y);
    float		g_fOffset5			: packoffset(c14.z);
    float		g_fOffset6			: packoffset(c14.w);
	float4		g_vAfterImgColor	: packoffset(c15);
	float4		g_vEmissiveColor	: packoffset(c16);
};

#define MAX_PALETTE 128
cbuffer cbSkinningMatrix : register(b2)
{
	float4x4 g_matBoneOffset[MAX_PALETTE];
	float4x4 g_matBoneScale[MAX_PALETTE];
	float4x4 g_matBoneRotation[MAX_PALETTE];
	float4x4 g_matBoneTrans[MAX_PALETTE];
	float4x4 g_matParentTransform[MAX_PALETTE];
	float4x4 g_matRootTransform[MAX_PALETTE];
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
};

/*__________________________________________________________________________________________________________
[ 그림자 (X) ]
____________________________________________________________________________________________________________*/
VS_OUT VS_MAIN(VS_IN vs_input)
{
	float4x4 matBone = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	
	if (vs_input.BoneWeights[0].x != 0.0f)
	{
	
		matBone  = mul(mul(g_matBoneOffset[vs_input.BoneId[0].x], mul(mul(mul(g_matBoneScale[vs_input.BoneId[0].x], g_matBoneRotation[vs_input.BoneId[0].x]), g_matBoneTrans[vs_input.BoneId[0].x]), g_matParentTransform[vs_input.BoneId[0].x])), g_matRootTransform[vs_input.BoneId[0].x]) * vs_input.BoneWeights[0].x;
		
		matBone += mul(mul(g_matBoneOffset[vs_input.BoneId[0].y], mul(mul(mul(g_matBoneScale[vs_input.BoneId[0].y], g_matBoneRotation[vs_input.BoneId[0].y]), g_matBoneTrans[vs_input.BoneId[0].y]), g_matParentTransform[vs_input.BoneId[0].y])), g_matRootTransform[vs_input.BoneId[0].y]) * vs_input.BoneWeights[0].y;
		
		matBone += mul(mul(g_matBoneOffset[vs_input.BoneId[0].z], mul(mul(mul(g_matBoneScale[vs_input.BoneId[0].z], g_matBoneRotation[vs_input.BoneId[0].z]), g_matBoneTrans[vs_input.BoneId[0].z]), g_matParentTransform[vs_input.BoneId[0].z])), g_matRootTransform[vs_input.BoneId[0].z]) * vs_input.BoneWeights[0].z;
		
		matBone += mul(mul(g_matBoneOffset[vs_input.BoneId[1].x], mul(mul(mul(g_matBoneScale[vs_input.BoneId[1].x], g_matBoneRotation[vs_input.BoneId[1].x]), g_matBoneTrans[vs_input.BoneId[1].x]), g_matParentTransform[vs_input.BoneId[1].x])), g_matRootTransform[vs_input.BoneId[1].x]) * vs_input.BoneWeights[1].x;
	}
	
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	float4 vModelPos	= mul(float4(vs_input.Pos, 1.0f), matBone);
	vs_output.Pos		= mul(vModelPos, matWVP);
	vs_output.TexUV		= vs_input.TexUV;
	vs_output.Normal	= vs_input.Normal;
	
	// N
	float3 WorldNormal		= mul(vs_input.Normal, (float3x3) g_matWorld);
	vs_output.N				= normalize(WorldNormal);
	
	// T
	float3 Tangent			= cross(float3(0.f, 1.f, 0.f), (float3) vs_input.Normal);
	float3 WorldTangent		= mul(Tangent, (float3x3) g_matWorld);
	vs_output.T				= normalize(WorldTangent);
	
	// B
	float3 Binormal			= cross((float3) vs_input.Normal, Tangent);
	float3 WorldBinormal	= mul(Binormal, (float3x3) g_matWorld);
	vs_output.B				= normalize(WorldBinormal);
	
	// ProjPos
	vs_output.ProjPos		= vs_output.Pos;
    vs_output.WorldPos = mul(float4(vs_input.Pos, 1.f), g_matWorld);
	return (vs_output);
}

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse		: SV_TARGET0;	
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
    ps_output.Diffuse = float4(1, 0, 0, 1);
	return (ps_output);
}