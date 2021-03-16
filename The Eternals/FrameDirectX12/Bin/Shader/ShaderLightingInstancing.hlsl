
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
typedef struct tagShaderLighting
{
	float4x4	matViewInv;
	float4x4	matProjInv;
	float4		vCameraPos;
	float4		vProjFar;
	float4		vLightDiffuse;
	float4		vLightSpecular;
	float4		vLightAmbient;
	float4		vLightDir;
	float4		vLightPos;
	float4		vLightRange;
	
} SHADER_LIGHTING;
StructuredBuffer<SHADER_LIGHTING> g_ShaderLighting : register(t0, space1);


// VS_MAIN
struct VS_IN
{
	float3 Pos		: POSITION;
	float2 TexUV	: TEXCOORD;
};

struct VS_OUT
{
	float4		Pos				: SV_POSITION;
	float2		TexUV			: TEXCOORD0;
	float4x4	ViewInv			: TEXCOORD1;
	float4x4	ProjInv			: TEXCOORD5;
	float4		CameraPos		: TEXCOORD9;
	float4		ProjFar			: TEXCOORD10;
	float4		LightDiffuse	: TEXCOORD11;
	float4		LightSpecular	: TEXCOORD12;
	float4		LightAmbient	: TEXCOORD13;
	float4		LightDir		: TEXCOORD14;
	float4		LightPos		: TEXCOORD15;
	float4		LightRange		: TEXCOORD16;
};

VS_OUT VS_MAIN(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output = (VS_OUT) 0;
	
	vs_output.Pos	= float4(vs_input.Pos, 1.f);
	vs_output.TexUV = vs_input.TexUV;
	
	vs_output.ViewInv       = g_ShaderLighting[iInstanceID].matViewInv;
	vs_output.ProjInv       = g_ShaderLighting[iInstanceID].matProjInv;
	vs_output.CameraPos     = g_ShaderLighting[iInstanceID].vCameraPos;
	vs_output.ProjFar       = g_ShaderLighting[iInstanceID].vProjFar;
	vs_output.LightDiffuse  = g_ShaderLighting[iInstanceID].vLightDiffuse;
	vs_output.LightSpecular = g_ShaderLighting[iInstanceID].vLightSpecular;
	vs_output.LightAmbient  = g_ShaderLighting[iInstanceID].vLightAmbient;
	vs_output.LightDir      = g_ShaderLighting[iInstanceID].vLightDir;
	vs_output.LightPos      = g_ShaderLighting[iInstanceID].vLightPos;
	vs_output.LightRange    = g_ShaderLighting[iInstanceID].vLightRange;
	
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
	- saturate() : 0 ~ 1������ �Ѿ�� ���� �߶�.
	____________________________________________________________________________________________________________*/
	// ���� ������ (0, 1)UV ��ǥ���� (-1 ~ 1)���� ��ǥ�� Ȯ��.
	float4 TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	float4 Normal		= vector(TexNormal.xyz * 2.f - 1.f, 0.f);
	
	float4 vDirection	= ps_input.LightDir * -1.0f;
	float4 Shade		= saturate(dot(normalize(vDirection), Normal));
	ps_output.Shade		= (ps_input.LightDiffuse) * (Shade + (ps_input.LightAmbient));
	ps_output.Shade.a	= 1.0f;
	
	/*__________________________________________________________________________________________________________
	[ Specular ]
	- Depth.x	posWVP.z / posWVP.w => Proj ������ Z��.
	- Depth.y	posWVP.w / Far		=> 0 ~ 1�� �������� View������ Z.
	- ViewZ		0 ~ 1 ���� �ٽ� Far�� ���Ͽ� 0 ~ Far���� ���� ����.
	____________________________________________________________________________________________________________*/
	float4	Depth	= g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
	float ViewZ		= Depth.y * ps_input.ProjFar.x;
	float4	Pos		= Depth;
	
	// (0, 0) ~ (1, 1) => (-1, 1) ~ (1, -1)
	Pos.x = (ps_input.TexUV.x *  2.0f - 1.0f) * ViewZ;	// UV��ǥ -> ������ǥ * ViewZ
	Pos.y = (ps_input.TexUV.y * -2.0f + 1.0f) * ViewZ;	// UV��ǥ -> ������ǥ * ViewZ
	Pos.z = Depth.x * ViewZ;							// ���������� z�� * ViewZ
	Pos.w = ViewZ;
	
	Pos = mul(Pos, ps_input.ProjInv); // Proj�� ������� ���Ͽ� View������ Pos.
	Pos = mul(Pos, ps_input.ViewInv); // View�� ������� ���Ͽ� World������ Pos.
	
	float3 LightDir		= normalize(vDirection.xyz);
	float3 Reflection	= normalize(reflect(LightDir, Normal.xyz));
	float3 Look = normalize(Pos.xyz - ps_input.CameraPos.xyz);
	
	float4	TexSpecular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	TexSpecular			= TexSpecular * 2.0f - 1.0f;
	float	SpecPower	= 1.15f;
	ps_output.Specular	= pow(saturate(dot(Reflection, -Look)), 0.0f) * (ps_input.LightSpecular * TexSpecular * SpecPower);
	
	return (ps_output);
	
};

// PS_POINT
PS_OUT PS_POINT(VS_OUT ps_input)
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	/*__________________________________________________________________________________________________________
	[ Point Lighting ]
	- Depth.x	posWVP.z / posWVP.w => Proj ������ Z��.
	- Depth.y	posWVP.w / Far		=> 0 ~ 1�� �������� View������ Z.
	- ViewZ		0 ~ 1 ���� �ٽ� Far�� ���Ͽ� 0 ~ Far���� ���� ����.
	____________________________________________________________________________________________________________*/
	// ���� ������ (0, 1)UV ��ǥ���� (-1 ~ 1)���� ��ǥ�� Ȯ��.
	float4	TexNormal	= g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
	float4	Normal		= float4(TexNormal.xyz * 2.f - 1.f, 0.f);
	
	float4	Depth		= g_TexDepth.Sample(g_samLinearWrap, ps_input.TexUV);
	float ViewZ			= Depth.y * ps_input.ProjFar.x;
	
	// (0, 0) ~ (1, 1) => (-1, 1) ~ (1, -1)
	float4	Pos;
	Pos.x = (ps_input.TexUV.x *  2.0f - 1.0f) * ViewZ;	// UV��ǥ -> ������ǥ * ViewZ
	Pos.y = (ps_input.TexUV.y * -2.0f + 1.0f) * ViewZ;	// UV��ǥ -> ������ǥ * ViewZ
	Pos.z = Depth.x * ViewZ;							// ���������� z�� * ViewZ
	Pos.w = ViewZ;
	
	Pos	= mul(Pos, ps_input.ProjInv);	// Proj�� ������� ���Ͽ� View������ Pos.
	Pos = mul(Pos, ps_input.ViewInv);	// View�� ������� ���Ͽ� World������ Pos.
	
	float4	LightDir	= Pos - ps_input.LightPos;
	float	Distnace	= length(LightDir);
	float	Att			= saturate((ps_input.LightRange.x - Distnace) / ps_input.LightRange.x);
	
	// Shade
	float4 Shade		= saturate(dot(normalize(LightDir) * -1.0f, Normal));
	ps_output.Shade		= (Shade + ps_input.LightAmbient) * ps_input.LightDiffuse * Att;
	ps_output.Shade.a	= 1.0f;
	
	
	// Specular
	float3 Reflection	= normalize(reflect(normalize(LightDir.xyz), Normal.xyz));
	float3 Look			= normalize(Pos.xyz - ps_input.CameraPos.xyz);
	
	float4	TexSpecular	= g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
	TexSpecular			= TexSpecular * 2.0f - 1.0f;
	float	SpecPower	= 1.15f;
	ps_output.Specular	= pow(saturate(dot(Reflection, -Look)), 0.0f) * (ps_input.LightSpecular * TexSpecular * Att) * SpecPower;
	
	return (ps_output);
}