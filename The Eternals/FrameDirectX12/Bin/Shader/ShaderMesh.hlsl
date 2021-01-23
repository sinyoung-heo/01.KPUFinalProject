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

/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView;
	float4x4	g_matProj;
	float4		g_vCameraPos;
	float		g_fProjFar;
}

cbuffer cbShaderMesh : register(b1)
{
	float4x4	g_matWorld;
	
	float4x4	g_matLightView;
	float4x4	g_matLightProj;
	float4		g_vLightPos;
	float		g_fLightPorjFar;
};

cbuffer cbSkinningMatrix : register(b2)
{
	float4x4	g_matBoneOffset[64];
	float4x4	g_matBoneScale[64];
	float4x4	g_matBoneRotation[64];
	float4x4	g_matBoneTrans[64];
	float4x4	g_matParentTransform[64];
	float4x4	g_matRootTransform[64];
};

/*__________________________________________________________________________________________________________
[ Global ]
____________________________________________________________________________________________________________*/
static float fFar = 1000.0f;



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
	
	return (vs_output);
}

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse		: SV_TARGET0;	// 0번 RenderTarget - Diffuse
	float4 Normal		: SV_TARGET1;	// 1번 RenderTarget - Normal
	float4 Specular		: SV_TARGET2;	// 2번 RenderTarget - Specular
	float4 Depth		: SV_TARGET3;	// 3번 RenderTarget - Depth
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;				// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);	// 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar,			// posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 0.0f, 1.0f);

	return (ps_output);
}

/*__________________________________________________________________________________________________________
[ 그림자 (O) ]
____________________________________________________________________________________________________________*/
VS_OUT VS_SHADOW_MAIN(VS_IN vs_input)
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
	
	// 광원 위치에서의 투영 좌표
	float4 matLightPosW		= mul(float4(vs_input.Pos.xyz, 1.0f), g_matWorld);
	float4 matLightPosWV	= mul(float4(matLightPosW.xyz, 1.0f), g_matLightView);
	vs_output.LightPos		= mul(float4(matLightPosWV.xyz, 1.0f), g_matLightProj);
	vs_output.LightPos.z	= vs_output.LightPos.z * vs_output.LightPos.w / g_fLightPorjFar;
	
	return (vs_output);
}

PS_OUT PS_SHADOW_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;				// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);	// 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj 영역의 Z.
								 ps_input.ProjPos.w / g_fProjFar,			// posWVP.w / Far : 0~1로 만든 View영역의 Z.
								 0.0f, 1.0f);
	
	/*__________________________________________________________________________________________________________
	[ 현재의 깊이와 그림자 깊이 비교 ]
	____________________________________________________________________________________________________________*/
	float2 uv = ps_input.LightPos.xy / ps_input.LightPos.w;
	uv.y = uv.y * -0.5f + 0.5f;
	uv.x = uv.x * 0.5f + 0.5f;
	
	float CurrentDepth	= ps_input.LightPos.z / ps_input.LightPos.w;
	float ShadowDepth	= g_TexShadowDepth.Sample(g_samLinearWrap, uv).x;
	if (CurrentDepth > ShadowDepth + 0.0000125f)
		ps_output.Diffuse.rgb *= 0.5;
	
	// ps_output.Diffuse.rgb = float3(1, 1, 1);
	

	return (ps_output);
}
