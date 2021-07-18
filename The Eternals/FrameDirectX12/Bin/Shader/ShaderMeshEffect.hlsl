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
Texture2D g_TexDiffuse : register(t0); // Diffuse 색상.
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
    float4      g_vColorOffSet1      : packoffset(c17);
};
struct VS_IN
{
	float3 Pos				: POSITION;
	float3 Normal			: NORMAL;
	float2 TexUV			: TEXCOORD;
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
    float3 WorldNormal : TEXCOORD9;
};

// PS_MAIN
struct PS_OUT
{
    float4 Effect0 : SV_TARGET0; // 
    float4 Effect1 : SV_TARGET1; // 
    float4 Effect2 : SV_TARGET2; // 
    float4 Effect3 : SV_TARGET3; // 
    float4 Effect4 : SV_TARGET4; // 
};

float2 rotateUV(float2 uv, float degrees)
{
    const float Deg2Rad = (3.14 * 2.0) / 360.0; //1도의 라디안값을 구한다
    float rotationRadians = degrees * Deg2Rad; //원하는 각도(디그리)의 라디안 값을 구한다
    float s = sin(rotationRadians);
    float c = cos(rotationRadians);
    float2x2 rotationMatrix = float2x2(c, -s, s, c); //회전 2차원 행렬을 만든다
    uv -= 0.5; //중심축을 이동시켜 가운데로 만들고
    uv = mul(rotationMatrix, uv); //회전한 다음에
    uv += 0.5; //다시 중심축 이동시켜 제자리로 만든다
    return uv;
}

/*__________________________________________________________________________________________________________
[ 그림자 (X) ]
____________________________________________________________________________________________________________*/
VS_OUT VS_MAIN(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
    float4x4 matworld = g_matWorld;
    float4x4 matWV, matWVP;
    matWV = mul(matworld, g_matView);
    matWVP = mul(matWV, g_matProj);	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;
    
    vs_output.AniUV = vs_input.TexUV + float2(0, -g_fOffset1 *0.3f);
    vs_output.WorldPos = mul(float4(vs_input.Pos, 1.f), g_matWorld);
    vs_output.WorldNormal = normalize(mul(vs_input.Normal, (float3x3) g_matWorld));
    return (vs_output);
}
VS_OUT VS_ANIUV(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
    float4x4 matworld = g_matWorld;
    float4x4 matWV, matWVP;
    matWV = mul(matworld, g_matView);
    matWVP = mul(matWV, g_matProj);
	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;
    
    vs_output.AniUV = vs_input.TexUV + float2(g_fOffset1, -g_fOffset1 * 0.3f);
    return (vs_output);
}
VS_OUT VS_ANIUV_X(VS_IN vs_input)
{
    VS_OUT vs_output = (VS_OUT) 0;
    float4x4 matworld = g_matWorld;
    float4x4 matWV, matWVP;
    matWV = mul(matworld, g_matView);
    matWVP = mul(matWV, g_matProj);
	
    vs_output.Pos = mul(float4(vs_input.Pos, 1.0f), matWVP);
    vs_output.TexUV = vs_input.TexUV;
    vs_output.Normal = vs_input.Normal;

    
    vs_output.AniUV = vs_input.TexUV + float2(g_fOffset1, g_fOffset5);
    return (vs_output);
}

PS_OUT PS_MAGIC_CIRCLE_RGB(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    clip(0.5f-distance(ps_input.TexUV, float2(0.5f, 0.5f)));
    float4 DR = g_TexDiffuse.Sample(g_samLinearWrap, rotateUV(ps_input.TexUV,g_fOffset3*0.25)).r; 
    float4 DG = g_TexDiffuse.Sample(g_samLinearWrap, rotateUV(ps_input.TexUV, g_fOffset3*0.5)).g;
    float4 DB = g_TexDiffuse.Sample(g_samLinearWrap, rotateUV(ps_input.TexUV, g_fOffset3)).b;
    float4 N = g_TexNormal.Sample(g_samLinearWrap, rotateUV(ps_input.TexUV, g_fOffset3));
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, rotateUV(ps_input.TexUV, g_fOffset3));
    DR = mul(DR, N);
    DG = mul(DG, N);
    DB = mul(DB, N); 
    DR.a =  g_fOffset1-0.3f;
    DG.a =  g_fOffset1;
    DB.a =  g_fOffset1 - 0.2f;
    float4 color = DR+DG+DB;
    ps_output.Effect2 = color;
    ps_output.Effect2.a = 0.5f;
    return ps_output;
}
PS_OUT PS_MAGIC_CIRCLE(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    float4 Diffuse = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 TexNormal = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Spec = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = lerp(lerp(Diffuse, Spec, 0.5), TexNormal, 0.5);
    color += mul(Diffuse,0.5f);
    ps_output.Effect2 = color;
    ps_output.Effect2.a = 0.5f;
    return ps_output;
}

PS_OUT PS_RAINDROP(VS_OUT ps_input) : SV_TARGET
{
   
    PS_OUT ps_output = (PS_OUT) 0;
	
	// Diffuse
    ps_output.Effect4 = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    //ps_output.Effect4 = (g_TexDiffuse.Sample(g_samLinearWrap, ps_input.AniUV * 10.f));
    ps_output.Effect4.a = 1.f;
    return (ps_output);
}
PS_OUT PS_EFFECT_SHPERE(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 ViewDir = normalize(g_vCameraPos - ps_input.WorldPos);
    float Lim = saturate(dot(ViewDir, float4(ps_input.WorldNormal, 1)));
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.AniUV );
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV * 3);
    float4 Dis = g_TexDissolve.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 Sha = g_TexShadowDepth.Sample(g_samLinearWrap, ps_input.TexUV * 3); //꽉찬그리드
    
    // g_vAfterImgColor  // g_vEmissiveColor // vColorOffSet
    float4 GridColor = mul(float4(0.3, 0.3, 0.7f, 1), S.r)  + mul(float4(0.7, 0.3, 0.7f, 1), Sha.r)* 2;
    float4 color =N+ mul(GridColor, Dis.r)*2 + mul(mul(float4(0.0, 0.3, 1, 0.5), g_fDissolve), 1 - Lim);
    ps_output.Effect2.rgba = color; //color.rgba; // +mul(float4(0.3, 0.4, 0.8, 1), Sha.r);
    ps_output.Effect2.a = 0.5f;
    return (ps_output);
}
PS_OUT PS_EFFECT_SHPERE_COLOR(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 ViewDir = normalize(g_vCameraPos - ps_input.WorldPos);
    float Lim = saturate(dot(ViewDir, float4(ps_input.WorldNormal, 1)));
    Lim= pow(Lim, 2);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV * 3);
    float4 Dis = g_TexDissolve.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 Sha = g_TexShadowDepth.Sample(g_samLinearWrap, ps_input.TexUV * 3); //꽉찬그리드
    float4 GridColor = mul(g_vAfterImgColor, S.r) + mul(g_vEmissiveColor, Sha.r) * 2;
    float4 color = mul(GridColor, Dis.r) * 2 + mul(mul(g_vColorOffSet1, 1 - Lim),g_fOffset6);
    ps_output.Effect2.rgba = color;
    ps_output.Effect2.a = g_fOffset3;
    return (ps_output);
}
PS_OUT PS_EFFECT_SHPERE_RED(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 ViewDir = normalize(g_vCameraPos - ps_input.WorldPos);
    float Lim = saturate(dot(ViewDir, float4(ps_input.WorldNormal, 1)));
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV * 3);
    float4 Dis = g_TexDissolve.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 Sha = g_TexShadowDepth.Sample(g_samLinearWrap, ps_input.TexUV * 3); //꽉찬그리드
    float4 GridColor = mul(float4(0.0, 0.3, 0.3f, 1), S.r) + mul(float4(0.0, 0.0, 0.7f, 1), Sha.r) * 2;
    float4 color = N + mul(GridColor, Dis.r) * 2 + mul(mul(float4(0.7, 0.3, 0, 0.5), g_fDissolve), 1 - Lim);
    ps_output.Effect2.rgba = color; //color.rgba; // +mul(float4(0.3, 0.4, 0.8, 1), Sha.r);
    ps_output.Effect2.a = 0.5f;
    return (ps_output);
}
PS_OUT PS_EFFECT_ICE_M(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 ViewDir = normalize(g_vCameraPos - ps_input.WorldPos);
    float Lim = saturate(dot(ViewDir, float4(ps_input.WorldNormal, 1)));
    Lim = pow(Lim, 2);
    float4 color = N+mul(mul(g_vColorOffSet1, 1 - Lim), g_fOffset6);
    ps_output.Effect2.rgba = color;
    ps_output.Effect2.a = g_fOffset3;
    return (ps_output);
}

PS_OUT PS_ICESTORM(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
	
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 E = float4(0, 0, 0, 1);
    float Normal_fDissolve = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV).r;

    if ((0.05f > (1.f - g_fDissolve) - Normal_fDissolve) && ((1.f - g_fDissolve) - Normal_fDissolve) > 0.f)
    {
        E = N;
    }
    if ((0.05f > (1.f - g_fOffset1) - Normal_fDissolve) && ((1.f - g_fOffset1) - Normal_fDissolve) > 0.f)
    {
        E += N;
    }
    clip((1.f - g_fDissolve) - Normal_fDissolve);
    
    
    
    ps_output.Effect1 = lerp((D + N), E, 0.5f);
    ps_output.Effect1.a = 0.5f;
    return (ps_output);
}
PS_OUT PS_DECAL(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= g_fOffset1; //강도
    float2 NewUV = float2(ps_input.TexUV.x + vDistortion.x * vDistortionInfo.b, ps_input.TexUV.y + vDistortion.y * vDistortionInfo.b);
    float2 AniUV = ps_input.AniUV; 
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, NewUV *2);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = mul(D.r, N) + mul(D.g, float4(0.6 + g_fOffset2, g_fOffset2, 0, 1)) + mul(D.b, float4(1.f, 0.5, 0.5, 1));
    ps_output.Effect2 = color;
    ps_output.Effect2.a = g_fOffset6;
    return (ps_output);
}
PS_OUT PS_ICEDECAL(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= g_fOffset1; //강도
    float2 NewUV = float2(ps_input.TexUV.x + vDistortion.x * vDistortionInfo.b, ps_input.TexUV.y + vDistortion.y * vDistortionInfo.b);
    float2 AniUV = ps_input.AniUV;
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, NewUV * 2);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = mul(D.r, N) + mul(D.g, float4(0, g_fOffset2, 0.6 + g_fOffset2, 1)) + mul(D.b, float4(0.5, 0.5, 1, 1));
    ps_output.Effect2 = color;
    ps_output.Effect2.a = g_fOffset6;
    return (ps_output);
}
PS_OUT PS_RECTDECAL(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.AniUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= g_fOffset1; //강도
    float2 NewUV = float2(ps_input.AniUV.x + vDistortion.x * vDistortionInfo.b, ps_input.AniUV.y + vDistortion.y * vDistortionInfo.b);
    float2 AniUV = ps_input.AniUV;
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, NewUV * 10);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = mul(D.g, N) + mul(D.r, float4(0.5, 0.5, 1, 1));
    ps_output.Effect2 = color;
    ps_output.Effect2.a = g_fOffset6;
    return (ps_output);
}
PS_OUT PS_BREATH(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV);
    float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
    vDistortion *= g_fOffset1; //강도
    float2 NewUV = float2(ps_input.AniUV.x + vDistortion.x * vDistortionInfo.b, ps_input.AniUV.y + vDistortion.y * vDistortionInfo.b);
    
    float2 AniUV = ps_input.AniUV;
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.AniUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, NewUV * 2.f);
    float4 Sha = g_TexShadowDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 color = D + N;
    ps_output.Effect2 = color;
    ps_output.Effect2.a = g_fOffset6;
    return (ps_output);
}

PS_OUT PS_MAIN(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    ps_output.Effect3 = D;
    ps_output.Effect3.a = g_fOffset1;
    return (ps_output);
}




PS_OUT PS_MAIN_SPRITE(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
  
    float u = (ps_input.AniUV.x / g_vAfterImgColor.x) + g_vAfterImgColor.y * (1.0f / g_vAfterImgColor.x);
    float v = (ps_input.AniUV.y / g_vAfterImgColor.z) + g_vAfterImgColor.w * (1.0f / g_vAfterImgColor.z);

    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV * g_fOffset5);
    D += g_vEmissiveColor;
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
   
    float Radian = atan2((0.5f - ps_input.TexUV.y), (ps_input.TexUV.x - 0.5f));
    float Degree = degrees(Radian) +180.f; // 0~360값
    //if (g_fOffset3 -90< Degree && Degree < g_fOffset3 + 90)
    //{
        vector vDistortionInfo = g_TexDissolve.Sample(g_samLinearWrap, ps_input.TexUV);
        float2 vDistortion = (vDistortionInfo.xy * 2.f) - 1.f;
        vDistortion *= g_fOffset1; //강도
       // float2 NewUV = float2(u + vDistortion.x * vDistortionInfo.b, v + vDistortion.y * vDistortionInfo.b);
   
    ps_output.Effect4 = mul(mul(S, D.r), 3.f);
    ps_output.Effect4.a = 1.f - g_fOffset6;
    return (ps_output);
}
PS_OUT PS_WARNING_GROUND(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 D = float4(1, 0, 0, 1);
    float4 N = g_TexNormal.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.TexUV);
    
    float4 color = mul(D, S.a); /* + mul(float4(1, 0, 0, 1), N);*/
    ps_output.Effect1 = float4(mul(float3(mul(1 - color.r, 2.f), 0, 0), S.a), g_fOffset1);
    return (ps_output);
}

PS_OUT PS_MAIN_SPRITE2(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float u = (ps_input.TexUV.x / g_vAfterImgColor.x) + g_vAfterImgColor.y * (1.0f / g_vAfterImgColor.x);
    float v = (ps_input.TexUV.y / g_vAfterImgColor.z) + g_vAfterImgColor.w * (1.0f / g_vAfterImgColor.z);
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, float2(u , v));
    ps_output.Effect3 = mul(D, 2.f);
    ps_output.Effect3.a = 1.f - g_fOffset6;
    return (ps_output);
}
PS_OUT PS_BOSSDECAL(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float2 DetailUV = float2(ps_input.TexUV.x, ps_input.TexUV.y * 5.f);
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, DetailUV);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.AniUV);
    ps_output.Effect3.xyz = mul(D.xyz, saturate(S.r));
    ps_output.Effect3.a = g_fOffset6;
    return (ps_output);
}
PS_OUT PS_GROUNDEFFECT(VS_OUT ps_input) : SV_TARGET
{
    PS_OUT ps_output = (PS_OUT) 0;
    float4 D = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.AniUV*5.f);
    float4 Dt = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 N = g_TexDiffuse.Sample(g_samLinearWrap, ps_input.AniUV * 2.f);
    float4 S = g_TexSpecular.Sample(g_samLinearWrap, ps_input.AniUV);
    
    float4 Sha = g_TexShadowDepth.Sample(g_samLinearWrap, ps_input.TexUV);
    float4 Color =mul(Dt, Sha.r);
    Color.a = g_fOffset4;
    ps_output.Effect3.xyz = mul(lerp(D, N, 0.5), S.r);
    ps_output.Effect3.a = g_fOffset6;
    
    ps_output.Effect3 = lerp(ps_output.Effect3, Color, 0.5f);
    return (ps_output);
}
