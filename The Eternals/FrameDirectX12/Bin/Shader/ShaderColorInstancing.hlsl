/*__________________________________________________________________________________________________________
[ Shader Resource ]
____________________________________________________________________________________________________________*/
typedef struct tagShaderTexture
{
	float4x4	matWorld;
	float4		vColor;
	
} SHADER_COLOR;
StructuredBuffer<SHADER_COLOR> g_ShaderColor : register(t0, space1);


cbuffer cbCamreaMatrix : register(b0)
{
	float4x4	g_matView		: packoffset(c0);
	float4x4	g_matProj		: packoffset(c4);
	float4		g_vCameraPos	: packoffset(c8);
	float		g_fProjFar		: packoffset(c9);
}


/*__________________________________________________________________________________________________________
[ Vertex Shader ]
____________________________________________________________________________________________________________*/
struct VS_IN
{
	float3 Pos		: POSITION;
	float4 Color	: COLOR0;
};

struct VS_OUT
{
	float4 Pos		: SV_POSITION;
	float4 Color	: COLOR0;
};

// VS Input Color
VS_OUT VS_MAIN(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_ShaderColor[iInstanceID].matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.Color		= g_ShaderColor[iInstanceID].vColor;
	
	return (vs_output);
}

// VS Random Color
VS_OUT VS_MAIN_RANDOM(VS_IN vs_input, uint iInstanceID : SV_InstanceID)
{
	VS_OUT vs_output	= (VS_OUT) 0;
	
	float4x4 matWV, matWVP;
	matWV	= mul(g_ShaderColor[iInstanceID].matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	vs_output.Pos		= mul(float4(vs_input.Pos, 1.0f), matWVP);
	vs_output.Color		= vs_input.Color;
	
	return (vs_output);
}

/*__________________________________________________________________________________________________________
[ Pixel Shader ]
____________________________________________________________________________________________________________*/
struct PS_OUT
{
	float4 Diffuse : SV_TARGET0; // 0¹ø RenderTarget - Diffuse
};

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
	PS_OUT ps_output = (PS_OUT) 0;
	
	ps_output.Diffuse = ps_input.Color;
	
	return (ps_output);
}
