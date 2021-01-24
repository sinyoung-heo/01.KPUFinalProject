
/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbShaderShadow : register(b0)
{	
	float4x4	g_matWorld	: packoffset(c0);
	float4x4	g_matView	: packoffset(c4);
	float4x4	g_matProj	: packoffset(c8);
	float		g_fProjFar	: packoffset(c12.x);
};

cbuffer cbSkinningMatrix : register(b1)
{
	float4x4	g_matBoneOffset[64];
	float4x4	g_matBoneScale[64];
	float4x4	g_matBoneRotation[64];
	float4x4	g_matBoneTrans[64];
	float4x4	g_matParentTransform[64];
	float4x4	g_matRootTransform[64];
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
	float4 Pos		: SV_POSITION;
	float4 ProjPos	: TEXCOORD0;
};

/*__________________________________________________________________________________________________________
[ BUMP MAPPING ]
____________________________________________________________________________________________________________*/
VS_OUT VS_MAIN(VS_IN vs_input)
{
	float4x4 matBone = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	
	if (vs_input.BoneWeights[0].x != 0.0f)
	{
		// Offset * Scale * Rotation * Transform * ParentTransform * RootTransform
		float4x4 matOS, matOSR, matOSRT, matOSRTP, matOSRTPR;
		
		matOS		= mul(g_matBoneOffset[vs_input.BoneId[0].x], g_matBoneScale[vs_input.BoneId[0].x]);
		matOSR		= mul(matOS, g_matBoneRotation[vs_input.BoneId[0].x]);
		matOSRT		= mul(matOSR, g_matBoneTrans[vs_input.BoneId[0].x]);
		matOSRTP	= mul(matOSRT, g_matParentTransform[vs_input.BoneId[0].x]);
		matOSRTPR	= mul(matOSRTP, g_matRootTransform[vs_input.BoneId[0].x]);
		matBone		= matOSRTPR * vs_input.BoneWeights[0].x;
		
		matOS		= mul(g_matBoneOffset[vs_input.BoneId[0].y], g_matBoneScale[vs_input.BoneId[0].y]);
		matOSR		= mul(matOS, g_matBoneRotation[vs_input.BoneId[0].y]);
		matOSRT		= mul(matOSR, g_matBoneTrans[vs_input.BoneId[0].y]);
		matOSRTP	= mul(matOSRT, g_matParentTransform[vs_input.BoneId[0].y]);
		matOSRTPR	= mul(matOSRTP, g_matRootTransform[vs_input.BoneId[0].y]);
		matBone		+= matOSRTPR * vs_input.BoneWeights[0].y;
		
		matOS		= mul(g_matBoneOffset[vs_input.BoneId[0].z], g_matBoneScale[vs_input.BoneId[0].z]);
		matOSR		= mul(matOS, g_matBoneRotation[vs_input.BoneId[0].z]);
		matOSRT		= mul(matOSR, g_matBoneTrans[vs_input.BoneId[0].z]);
		matOSRTP	= mul(matOSRT, g_matParentTransform[vs_input.BoneId[0].z]);
		matOSRTPR	= mul(matOSRTP, g_matRootTransform[vs_input.BoneId[0].z]);
		matBone		+= matOSRTPR * vs_input.BoneWeights[0].z;

		matOS		= mul(g_matBoneOffset[vs_input.BoneId[1].x], g_matBoneScale[vs_input.BoneId[1].x]);
		matOSR		= mul(matOS, g_matBoneRotation[vs_input.BoneId[1].x]);
		matOSRT		= mul(matOSR, g_matBoneTrans[vs_input.BoneId[1].x]);
		matOSRTP	= mul(matOSRT, g_matParentTransform[vs_input.BoneId[1].x]);
		matOSRTPR	= mul(matOSRTP, g_matRootTransform[vs_input.BoneId[1].x]);
		matBone		+= matOSRTPR * vs_input.BoneWeights[1].x;
	}
	
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);
	
	float4 vModelPos	= mul(float4(vs_input.Pos, 1.0f), matBone);
	vs_output.Pos		= mul(vModelPos, matWVP);
	
	// 광원 위치에서의 투영 좌표 계산.
	float4 vPos			= vs_output.Pos;
	vPos.z				= vPos.z * vPos.w / g_fProjFar;
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
