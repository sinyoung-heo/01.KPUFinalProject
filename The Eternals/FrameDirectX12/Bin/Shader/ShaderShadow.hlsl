
/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbShadowDepth : register(b0)
{
	float4x4	matWVP			: packoffset(c0);
	float4x4	matWorld		: packoffset(c4);
	float4x4	matLightView	: packoffset(c8);
	float4x4	matLightProj	: packoffset(c12);

	float4		vLightPosition	: packoffset(c16);
	float		fLightPorjFar	: packoffset(c17);
};

cbuffer cbSkinningInfo : register(b1)
{
	float4x4 matBoneOffset[64];
	
	float4x4 matBoneScale[64];
	float4x4 matBoneRotation[64];
	float4x4 matBoneTrans[64];
	
	float4x4 matParentTransform[64];
	
	float4x4 matRootTransform[64];
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
		matBone = mul(mul(matBoneOffset[vs_input.BoneId[0].x], mul(mul(mul(matBoneScale[vs_input.BoneId[0].x], matBoneRotation[vs_input.BoneId[0].x]), matBoneTrans[vs_input.BoneId[0].x]), matParentTransform[vs_input.BoneId[0].x])), matRootTransform[vs_input.BoneId[0].x]) * vs_input.BoneWeights[0].x;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[0].y], mul(mul(mul(matBoneScale[vs_input.BoneId[0].y], matBoneRotation[vs_input.BoneId[0].y]), matBoneTrans[vs_input.BoneId[0].y]), matParentTransform[vs_input.BoneId[0].y])), matRootTransform[vs_input.BoneId[0].y]) * vs_input.BoneWeights[0].y;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[0].z], mul(mul(mul(matBoneScale[vs_input.BoneId[0].z], matBoneRotation[vs_input.BoneId[0].z]), matBoneTrans[vs_input.BoneId[0].z]), matParentTransform[vs_input.BoneId[0].z])), matRootTransform[vs_input.BoneId[0].z]) * vs_input.BoneWeights[0].z;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[1].x], mul(mul(mul(matBoneScale[vs_input.BoneId[1].x], matBoneRotation[vs_input.BoneId[1].x]), matBoneTrans[vs_input.BoneId[1].x]), matParentTransform[vs_input.BoneId[1].x])), matRootTransform[vs_input.BoneId[1].x]) * vs_input.BoneWeights[1].x;
	}
	
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4 vModelPos	= mul(float4(vs_input.Pos, 1.0f), matBone);
	vs_output.Pos		= mul(vModelPos, matWVP);
	
	// 광원 위치에서의 투영 좌표 계산.
	float4 vPos			= vs_output.Pos;
	vPos.z				= vPos.z * vPos.w / fLightPorjFar;
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
	
	ps_output.ShadowDepth = float4(Depth.xxx, 1.0f);
	
	return (ps_output);
}
