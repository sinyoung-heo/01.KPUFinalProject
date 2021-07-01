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
[ Shader Resource ]
____________________________________________________________________________________________________________*/
#define MAX_PALETTE 128
static float g_fProjFar = 1000.0f;

typedef struct tagShaderDynamicMesh
{
    float4x4    matWorld;
	float4x4    matView;
	float4x4    matProj;
    float       fDissolve;
    float4      vEmissiveColor;
    
} SHADER_DYNAMICMESH;
StructuredBuffer<SHADER_DYNAMICMESH> g_ShaderMesh : register(t0, space1);

typedef struct tagSkinningMatrix
{
	float4x4 matBoneOffset[MAX_PALETTE];
	float4x4 matBoneScale[MAX_PALETTE];
	float4x4 matBoneRotation[MAX_PALETTE];
	float4x4 matBoneTrans[MAX_PALETTE];
	float4x4 matParentTransform[MAX_PALETTE];
	float4x4 matRootTransform[MAX_PALETTE];
    
} SKINNING_MATRIX;
StructuredBuffer<SKINNING_MATRIX> g_SkinningMatrix : register(t1, space1);

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
	float4	Pos			: SV_POSITION;
	float2	TexUV		: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	
	float3	T			: TEXCOORD2;
	float3	B			: TEXCOORD3;
	float3	N			: TEXCOORD4;
	
	float4	ProjPos		: TEXCOORD5;
    float   Dissolve    : TEXCOORD6;
	float4  Emissive    : TEXCOORD7;
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


VS_OUT VS_MAIN(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	float4x4 matBone = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	
	if (vs_input.BoneWeights[0].x != 0.0f)
	{
		//// Offset * Scale * Rotation * Transform * ParentTransform * RootTransform
		//float4x4 matOS, matOSR, matOSRT, matOSRTP, matOSRTPR;
		
		//matOS = mul(g_ShaderMesh[iInstanceID].matBoneOffset[vs_input.BoneId[0].x], g_ShaderMesh[iInstanceID].matBoneScale[vs_input.BoneId[0].x]);
		//matOSR = mul(matOS, g_ShaderMesh[iInstanceID].matBoneRotation[vs_input.BoneId[0].x]);
		//matOSRT = mul(matOSR, g_ShaderMesh[iInstanceID].matBoneTrans[vs_input.BoneId[0].x]);
		//matOSRTP = mul(matOSRT, g_ShaderMesh[iInstanceID].matParentTransform[vs_input.BoneId[0].x]);
		//matOSRTPR = mul(matOSRTP, g_ShaderMesh[iInstanceID].matRootTransform[vs_input.BoneId[0].x]);
		//matBone = matOSRTPR * vs_input.BoneWeights[0].x;
		
		//matOS = mul(g_ShaderMesh[iInstanceID].matBoneOffset[vs_input.BoneId[0].y], g_ShaderMesh[iInstanceID].matBoneScale[vs_input.BoneId[0].y]);
		//matOSR = mul(matOS, g_ShaderMesh[iInstanceID].matBoneRotation[vs_input.BoneId[0].y]);
		//matOSRT = mul(matOSR, g_ShaderMesh[iInstanceID].matBoneTrans[vs_input.BoneId[0].y]);
		//matOSRTP = mul(matOSRT, g_ShaderMesh[iInstanceID].matParentTransform[vs_input.BoneId[0].y]);
		//matOSRTPR = mul(matOSRTP, g_ShaderMesh[iInstanceID].matRootTransform[vs_input.BoneId[0].y]);
		//matBone += matOSRTPR * vs_input.BoneWeights[0].y;
		
		//matOS = mul(g_ShaderMesh[iInstanceID].matBoneOffset[vs_input.BoneId[0].z], g_ShaderMesh[iInstanceID].matBoneScale[vs_input.BoneId[0].z]);
		//matOSR = mul(matOS, g_ShaderMesh[iInstanceID].matBoneRotation[vs_input.BoneId[0].z]);
		//matOSRT = mul(matOSR, g_ShaderMesh[iInstanceID].matBoneTrans[vs_input.BoneId[0].z]);
		//matOSRTP = mul(matOSRT, g_ShaderMesh[iInstanceID].matParentTransform[vs_input.BoneId[0].z]);
		//matOSRTPR = mul(matOSRTP, g_ShaderMesh[iInstanceID].matRootTransform[vs_input.BoneId[0].z]);
		//matBone += matOSRTPR * vs_input.BoneWeights[0].z;

		//matOS = mul(g_ShaderMesh[iInstanceID].matBoneOffset[vs_input.BoneId[1].x], g_ShaderMesh[iInstanceID].matBoneScale[vs_input.BoneId[1].x]);
		//matOSR = mul(matOS, g_ShaderMesh[iInstanceID].matBoneRotation[vs_input.BoneId[1].x]);
		//matOSRT = mul(matOSR, g_ShaderMesh[iInstanceID].matBoneTrans[vs_input.BoneId[1].x]);
		//matOSRTP = mul(matOSRT, g_ShaderMesh[iInstanceID].matParentTransform[vs_input.BoneId[1].x]);
		//matOSRTPR = mul(matOSRTP, g_ShaderMesh[iInstanceID].matRootTransform[vs_input.BoneId[1].x]);
		//matBone += matOSRTPR * vs_input.BoneWeights[1].x;
	}
	
	matBone = mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneOffset[vs_input.BoneId[0].x], mul(mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneScale[vs_input.BoneId[0].x], g_SkinningMatrix[iInstanceID]
	.matBoneRotation[vs_input.BoneId[0].x]), g_SkinningMatrix[iInstanceID]
	.matBoneTrans[vs_input.BoneId[0].x]), g_SkinningMatrix[iInstanceID]
	.matParentTransform[vs_input.BoneId[0].x])),
	g_SkinningMatrix[iInstanceID]
	.matRootTransform[vs_input.BoneId[0].x]) * vs_input.BoneWeights[0].x;
		
	matBone += mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneOffset[vs_input.BoneId[0].y], mul(mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneScale[vs_input.BoneId[0].y], g_SkinningMatrix[iInstanceID]
	.matBoneRotation[vs_input.BoneId[0].y]), g_SkinningMatrix[iInstanceID]
	.matBoneTrans[vs_input.BoneId[0].y]), g_SkinningMatrix[iInstanceID]
	.matParentTransform[vs_input.BoneId[0].y])),
	g_SkinningMatrix[iInstanceID]
	.matRootTransform[vs_input.BoneId[0].y]) * vs_input.BoneWeights[0].y;
		
	matBone += mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneOffset[vs_input.BoneId[0].z], mul(mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneScale[vs_input.BoneId[0].z], g_SkinningMatrix[iInstanceID]
	.matBoneRotation[vs_input.BoneId[0].z]), g_SkinningMatrix[iInstanceID]
	.matBoneTrans[vs_input.BoneId[0].z]), g_SkinningMatrix[iInstanceID]
	.matParentTransform[vs_input.BoneId[0].z])),
	g_SkinningMatrix[iInstanceID]
	.matRootTransform[vs_input.BoneId[0].z]) * vs_input.BoneWeights[0].z;
		
	matBone += mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneOffset[vs_input.BoneId[1].x], mul(mul(mul(g_SkinningMatrix[iInstanceID]
	.matBoneScale[vs_input.BoneId[1].x], g_SkinningMatrix[iInstanceID]
	.matBoneRotation[vs_input.BoneId[1].x]), g_SkinningMatrix[iInstanceID]
	.matBoneTrans[vs_input.BoneId[1].x]), g_SkinningMatrix[iInstanceID]
	.matParentTransform[vs_input.BoneId[1].x])),
	g_SkinningMatrix[iInstanceID]
	.matRootTransform[vs_input.BoneId[1].x]) * vs_input.BoneWeights[1].x;
	
	
	float4x4 matWV, matWVP;
	matWV  = mul(g_ShaderMesh[iInstanceID].matWorld, g_ShaderMesh[iInstanceID].matView);
	matWVP = mul(matWV, g_ShaderMesh[iInstanceID].matProj);
	
	float4 vModelPos = mul(float4(vs_input.Pos, 1.0f), matBone);
	vs_output.Pos    = mul(vModelPos, matWVP);
	vs_output.TexUV  = vs_input.TexUV;
	vs_output.Normal = vs_input.Normal;
	
	// N
	float3 WorldNormal = mul(vs_input.Normal, (float3x3) g_ShaderMesh[iInstanceID].matWorld);
	vs_output.N = normalize(WorldNormal);
	
	// T
	float3 Tangent = cross(float3(0.f, 1.f, 0.f), (float3) vs_input.Normal);
	float3 WorldTangent = mul(Tangent, (float3x3) g_ShaderMesh[iInstanceID].matWorld);
	vs_output.T = normalize(WorldTangent);
	
	// B
	float3 Binormal = cross((float3) vs_input.Normal, Tangent);
	float3 WorldBinormal = mul(Binormal, (float3x3) g_ShaderMesh[iInstanceID].matWorld);
	vs_output.B = normalize(WorldBinormal);
	
	// ProjPos
	 vs_output.ProjPos = vs_output.Pos;
	
	vs_output.Dissolve = g_ShaderMesh[iInstanceID].fDissolve;
	
	vs_output.Emissive = g_ShaderMesh[iInstanceID].vEmissiveColor;
	
	return (vs_output);
}

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV) * float4(ps_input.Emissive.xyz, 1.f);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;			// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);// 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar,			// posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 1.0f, 1.0f);
 
	return (ps_output);
}

PS_OUT PS_DISSOLVE(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;			// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);// 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar,			// posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 1.0f, 1.0f);
	
	/*__________________________________________________________________________________________________________
	[ Dissolve ]
	____________________________________________________________________________________________________________*/
	float Normal_fDissolve = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV).r;
	
	if ((0.05f > (1.f - ps_input.Dissolve) - Normal_fDissolve) && ((1.f - ps_input.Dissolve) - Normal_fDissolve) > 0.0f)
	{
		ps_output.Emissive = ps_input.Emissive;
	}
	clip((1.f - ps_input.Dissolve) - Normal_fDissolve);
	
	return (ps_output);
}