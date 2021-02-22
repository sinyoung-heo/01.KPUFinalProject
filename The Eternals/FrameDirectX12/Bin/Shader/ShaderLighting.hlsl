
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
Texture2D g_TexNormal	: register(t0);
Texture2D g_TexSpecular : register(t1);
Texture2D g_TexDepth	: register(t2);


/*____________________________________________________________________
[ Constant Buffer ]
______________________________________________________________________*/
cbuffer cbShaderLighting : register(b0)
{
	float4x4	g_matViewInv		: packoffset(c0);
	float4x4	g_matProjInv		: packoffset(c4);
	float4		g_vCameraPos		: packoffset(c8);
	float4		g_vProjFar			: packoffset(c9);
	
	float4		g_vLightDiffuse		: packoffset(c10);
	float4		g_vLightSpecular	: packoffset(c11);
	float4		g_vLightAmibient	: packoffset(c12);
	float4		g_vLightDir			: packoffset(c13);
	float4		g_vLightPos			: packoffset(c14);
	float4		g_vLightRange		: packoffset(c15);
									
};


/*__________________________________________________________________________________________________________
[ Global ]
____________________________________________________________________________________________________________*/
static float fFar = 1000.0f;



// VS_MAIN
struct VS_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_OUT
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN vs_input)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	vs_output.Pos	= float4(vs_input.Pos, 1.f);
	vs_output.TexUV = vs_input.TexUV;
	
	return vs_output;
}

// PS_DIRECTION
struct PS_OUT
{
	float4 Shade	: SV_TARGET0;
	float4 Specular : SV_TARGET1;
};

PS_OUT PS_DIRECTION(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	/*__________________________________________________________________________________________________________
	[ Direction Lighting ]
	- saturate() : 0 ~ 1범위를 넘어서는 값을 잘라냄.
	____________________________________________________________________________________________________________*/
	// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	float4 Normal		= vector(TexNormal.xyz * 2.f - 1.f, 0.f);
	
	float4 vDirection = g_vLightDir * -1.0f;
	float4 Shade		= saturate(dot(normalize(vDirection), Normal));
	ps_output.Shade		= (g_vLightDiffuse) * (Shade + (g_vLightAmibient));
	ps_output.Shade.a	= 1.0f;
	
	/*__________________________________________________________________________________________________________
	[ Specular ]
	- Depth.x	posWVP.z / posWVP.w => Proj 영역의 Z값.
	- Depth.y	posWVP.w / Far		=> 0 ~ 1로 만들어놓은 View영역의 Z.
	- ViewZ		0 ~ 1 값에 다시 Far를 곱하여 0 ~ Far깊이 값을 복구.
	____________________________________________________________________________________________________________*/
	float4	Depth	= g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
	
	float	ViewZ	= Depth.y * g_vProjFar.x;
	float4	Pos		= Depth;
	// (0, 0) ~ (1, 1) => (-1, 1) ~ (1, -1)
	Pos.x = (ps_input.TexUV.x *  2.0f - 1.0f) * ViewZ;	// UV좌표 -> 투영좌표 * ViewZ
	Pos.y = (ps_input.TexUV.y * -2.0f + 1.0f) * ViewZ;	// UV좌표 -> 투영좌표 * ViewZ
	Pos.z = Depth.x * ViewZ;							// 투영영역의 z값 * ViewZ
	Pos.w = ViewZ;
	
	Pos = mul(Pos, g_matProjInv);	// Proj의 역행렬을 곱하여 View영역의 Pos.
	Pos = mul(Pos, g_matViewInv);	// View의 역행렬을 곱하여 World영역의 Pos.
	
	float3 LightDir		= normalize(vDirection.xyz);
	float3 Reflection	= normalize(reflect(LightDir, Normal.xyz));
	float3 Look			= normalize(Pos.xyz - g_vCameraPos.xyz);
	
	float4	TexSpecular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	TexSpecular			= TexSpecular * 2.0f - 1.0f;
	float	SpecPower	= 1.15f;
	ps_output.Specular	= pow(saturate(dot(Reflection, -Look)), 0.0f) * (g_vLightSpecular * TexSpecular * SpecPower);
	
	return (ps_output);
	
};

// PS_POINT
PS_OUT PS_POINT(VS_OUT ps_input)
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	/*__________________________________________________________________________________________________________
	[ Point Lighting ]
	- Depth.x	posWVP.z / posWVP.w => Proj 영역의 Z값.
	- Depth.y	posWVP.w / Far		=> 0 ~ 1로 만들어놓은 View영역의 Z.
	- ViewZ		0 ~ 1 값에 다시 Far를 곱하여 0 ~ Far깊이 값을 복구.
	____________________________________________________________________________________________________________*/
	// 값의 범위를 (0, 1)UV 좌표에서 (-1 ~ 1)투영 좌표로 확장.
	float4	TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	float4	Normal		= float4(TexNormal.xyz * 2.f - 1.f, 0.f);
	
	float4	Depth		= g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
	float	ViewZ		= Depth.y * g_vProjFar.x;
	
	// (0, 0) ~ (1, 1) => (-1, 1) ~ (1, -1)
	float4	Pos;
	Pos.x = (ps_input.TexUV.x *  2.0f - 1.0f) * ViewZ;	// UV좌표 -> 투영좌표 * ViewZ
	Pos.y = (ps_input.TexUV.y * -2.0f + 1.0f) * ViewZ;	// UV좌표 -> 투영좌표 * ViewZ
	Pos.z = Depth.x * ViewZ;							// 투영영역의 z값 * ViewZ
	Pos.w = ViewZ;
	
	Pos	= mul(Pos, g_matProjInv);	// Proj의 역행렬을 곱하여 View영역의 Pos.
	Pos	= mul(Pos, g_matViewInv);	// View의 역행렬을 곱하여 World영역의 Pos.
	
	float4	LightDir = Pos - g_vLightPos;
	float	Distnace = length(LightDir);
	float	Att		 = saturate((g_vLightRange.x - Distnace) / g_vLightRange.x);
	
	// Shade
	float4 Shade		= saturate(dot(normalize(LightDir) * -1.0f, Normal));
	ps_output.Shade		= (Shade + g_vLightAmibient) * g_vLightDiffuse * Att;
	ps_output.Shade.a	= 1.0f;
	
	
	// Specular
	float3 Reflection	= normalize(reflect(normalize(LightDir.xyz), Normal.xyz));
	float3 Look			= normalize(Pos.xyz - g_vCameraPos.xyz);
	
	float4	TexSpecular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	TexSpecular			= TexSpecular * 2.0f - 1.0f;
	float	SpecPower	= 1.15f;
	ps_output.Specular  = pow(saturate(dot(Reflection, -Look)), 0.0f) * (g_vLightSpecular * TexSpecular * Att) * SpecPower;
	
	return (ps_output);
}