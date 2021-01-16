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
Texture2D g_TexDiffuse		: register(t0);	// Diffuse ����.
Texture2D g_TexNormal		: register(t1);	// ź��Ʈ ���� Normal Map.
Texture2D g_TexSpecular		: register(t2);	// Specular ����.
Texture2D g_TexShadowDepth	: register(t3);	// ShadowDepth

/*__________________________________________________________________________________________________________
[ Constant Buffer ]
____________________________________________________________________________________________________________*/
cbuffer cbMatrixInfo	: register(b0)
{
	float4x4 matWVP		: packoffset(c0);
	float4x4 matWorld	: packoffset(c4);
	float4x4 matView	: packoffset(c8);
	float4x4 matProj	: packoffset(c12);
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

cbuffer cbShadowInfo : register(b2)
{
	float4x4	matLightView	: packoffset(c0);
	float4x4	matLightProj	: packoffset(c4);
	float4		vLightPosition	: packoffset(c8);
	float		fLightPorjFar	: packoffset(c9);
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
[ BUMP MAPPING ]
____________________________________________________________________________________________________________*/
VS_OUT VS_MAIN(VS_IN vs_input)
{
	float4x4 matBone = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	
	if (vs_input.BoneWeights[0].x != 0.0f)
	{
		matBone  = mul(mul(matBoneOffset[vs_input.BoneId[0].x], mul(mul(mul(matBoneScale[vs_input.BoneId[0].x], matBoneRotation[vs_input.BoneId[0].x]), matBoneTrans[vs_input.BoneId[0].x]), matParentTransform[vs_input.BoneId[0].x])), matRootTransform[vs_input.BoneId[0].x]) * vs_input.BoneWeights[0].x;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[0].y], mul(mul(mul(matBoneScale[vs_input.BoneId[0].y], matBoneRotation[vs_input.BoneId[0].y]), matBoneTrans[vs_input.BoneId[0].y]), matParentTransform[vs_input.BoneId[0].y])), matRootTransform[vs_input.BoneId[0].y]) * vs_input.BoneWeights[0].y;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[0].z], mul(mul(mul(matBoneScale[vs_input.BoneId[0].z], matBoneRotation[vs_input.BoneId[0].z]), matBoneTrans[vs_input.BoneId[0].z]), matParentTransform[vs_input.BoneId[0].z])), matRootTransform[vs_input.BoneId[0].z]) * vs_input.BoneWeights[0].z;
		
		matBone += mul(mul(matBoneOffset[vs_input.BoneId[1].x], mul(mul(mul(matBoneScale[vs_input.BoneId[1].x], matBoneRotation[vs_input.BoneId[1].x]), matBoneTrans[vs_input.BoneId[1].x]), matParentTransform[vs_input.BoneId[1].x])), matRootTransform[vs_input.BoneId[1].x]) * vs_input.BoneWeights[1].x;
	}
	
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4 vModelPos	= mul(float4(vs_input.Pos, 1.0f), matBone);
	vs_output.Pos		= mul(vModelPos, matWVP);
	vs_output.TexUV		= vs_input.TexUV;
	vs_output.Normal	= vs_input.Normal;
	
	// N
	float3 WorldNormal		= mul(vs_input.Normal, (float3x3) matWorld);
	vs_output.N				= normalize(WorldNormal);
	
	// T
	float3 Tangent			= cross(float3(0.f, 1.f, 0.f), (float3) vs_input.Normal);
	float3 WorldTangent		= mul(Tangent, (float3x3) matWorld);
	vs_output.T				= normalize(WorldTangent);
	
	// B
	float3 Binormal			= cross((float3) vs_input.Normal, Tangent);
	float3 WorldBinormal	= mul(Binormal, (float3x3) matWorld);
	vs_output.B				= normalize(WorldBinormal);
	
	// ProjPos
	vs_output.ProjPos		= vs_output.Pos;
	
	// ���� ��ġ������ ���� ��ǥ
	float4 matLightPosW		= mul(float4(vs_input.Pos.xyz, 1.0f), matWorld);
	float4 matLightPosWV	= mul(float4(matLightPosW.xyz, 1.0f), matLightView);
	vs_output.LightPos		= mul(float4(matLightPosWV.xyz, 1.0f), matLightProj);
	vs_output.LightPos.z	= vs_output.LightPos.z * vs_output.LightPos.w / fLightPorjFar;
	
	return (vs_output);
}

// PS_MAIN
struct PS_OUT
{
	float4 Diffuse		: SV_TARGET0;	// 0�� RenderTarget - Diffuse
	float4 Normal		: SV_TARGET1;	// 1�� RenderTarget - Normal
	float4 Specular		: SV_TARGET2;	// 2�� RenderTarget - Specular
	float4 Depth		: SV_TARGET3;	// 3�� RenderTarget - Depth
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;				// ���� ������ (0, 1)UV ��ǥ���� (-1 ~ 1)���� ��ǥ�� Ȯ��.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);	// ���� ������ (0 ~ 1)UV ��ǥ�� �ٽ� ���.
	
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj ������ Z.
								 ps_input.ProjPos.w / fFar,				// posWVP.w / Far : 0~1�� ���� View������ Z.
								 0.0f, 1.0f);

	return (ps_output);
}


PS_OUT PS_SHADOW_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
	ps_output.Diffuse	= g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Normal
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;				// ���� ������ (0, 1)UV ��ǥ���� (-1 ~ 1)���� ��ǥ�� Ȯ��.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
	ps_output.Normal	= float4(Normal.xyz * 0.5f + 0.5f, 1.f);	// ���� ������ (0 ~ 1)UV ��ǥ�� �ٽ� ���.
	
	// Specular
	ps_output.Specular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	
	// Depth
	ps_output.Depth		= float4(ps_input.ProjPos.z / ps_input.ProjPos.w,	// (posWVP.z / posWVP.w) : Proj ������ Z.
								 ps_input.ProjPos.w / fFar,				// posWVP.w / Far : 0~1�� ���� View������ Z.
								 0.0f, 1.0f);
	
	/*__________________________________________________________________________________________________________
	[ ������ ���̿� �׸��� ���� �� ]
	____________________________________________________________________________________________________________*/
	float2 uv	= ps_input.LightPos.xy / ps_input.LightPos.w;
	uv.y		= uv.y * -0.5f + 0.5f;
	uv.x		= uv.x *  0.5f + 0.5f;
	
	float CurrentDepth	= ps_input.LightPos.z / ps_input.LightPos.w;
	float ShadowDepth	= g_TexShadowDepth.Sample(g_samLinearWrap, uv).x;
	if (CurrentDepth > ShadowDepth + 0.0000125f)
	{
		ps_output.Diffuse.rgb *= 0.5;
	}	
	
	return (ps_output);
}
