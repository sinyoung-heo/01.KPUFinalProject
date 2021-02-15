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
}

cbuffer cbShaderMesh : register(b1)
{
	float4x4	g_matWorld		: packoffset(c0);
	
	float4x4	g_matLightView	: packoffset(c4);
	float4x4	g_matLightProj	: packoffset(c8);
	float4		g_vLightPos		: packoffset(c12);
	float		g_fLightPorjFar	: packoffset(c13.x);
    float		g_fDissolve		: packoffset(c13.y);
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
    float4 Emissive		: SV_TARGET4;	// 4번 RenderTarget - Emissive
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

    //float Normal_fDissolve = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV).r;

    //if ((0.05f > (1.f - fDissolve) - Normal_fDissolve) && ((1.f - fDissolve) - Normal_fDissolve) > 0.f)
    //{
    //    ps_output.Emissive = float4(1, Normal_fDissolve, 0, 1);
    //}
    //clip((1.f - fDissolve) - Normal_fDissolve);

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


VS_OUT VS_DISTORTION(VS_IN vs_input)
{
  
    float4x4 matBone = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	
    VS_OUT vs_output = (VS_OUT) 0;
	
    float4x4 matWV, matWVP;
    matWV = mul(g_matWorld, g_matView);
    matWVP = mul(matWV, g_matProj);
	
    float4 vModelPos = mul(float4(vs_input.Pos, 1.0f), matBone);
    vs_output.Pos = mul(vModelPos, matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;
	

	// ProjPos
    vs_output.ProjPos = vs_output.Pos;
	
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
    return (vs_output);
}

float4 PS_DISTORTION(VS_OUT ps_input) : SV_TARGET0
{
	// Normal
    float4 psout;
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	TexNormal			= (TexNormal * 2.0f) - 1.0f;				// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float3 Normal		= (TexNormal.x * ps_input.T) + (TexNormal.y * ps_input.B) + (TexNormal.z * ps_input.N);
    psout = float4(Normal.xyz * 0.5f + 0.5f, 1.f); // 값의 범위를 (0 ~ 1)UV 좌표로 다시 축소.
	
    float2 TexUv = ps_input.TexUV;
    return g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
}
