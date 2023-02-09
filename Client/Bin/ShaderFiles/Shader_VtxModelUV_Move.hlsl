
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_GlowTexture;
texture2D		g_TestTexture;

float				g_iUV_Max_Width_Num;
float				g_iUV_Max_Height_Num;
float				g_iUV_Cur_Width_Num;
float				g_iUV_Cur_Height_Num;

float				G_Power;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV );

	Out.vDiffuse = vDiffuse;

	/* -1 ~ 1 => 0 ~ 1 */
	vector vNormal = vector(-1.f, 1.f, -1.f, 0.f);
	Out.vNormal = normalize(vNormal);//vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);

	if (0.4f > vDiffuse.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_Glow(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4	GlowColor, TexturColor;
	TexturColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV );
	GlowColor = g_GlowTexture.Sample(LinearSampler, In.vTexUV );

	Out.vDiffuse = saturate(TexturColor + (GlowColor* G_Power));

	if (Out.vDiffuse.r >= 0.65f && Out.vDiffuse.g >= 0.65f && Out.vDiffuse.b >= 0.65f)
		discard;

	if (0.25f > Out.vDiffuse.a)
		discard;


	return Out;
}

PS_OUT PS_MAIN_MeshUvCut(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vTest = g_TestTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse = vDiffuse + vTest;

	/* -1 ~ 1 => 0 ~ 1 */
	vector vNormal = vector(-1.f, 1.f, -1.f, 0.f);
	Out.vNormal = normalize(vNormal);//vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 0.f);

	if (0.4f > vDiffuse.a)
		discard;

	if (0.65f >= In.vTexUV.y)
		discard;


	return Out;
}


technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Glow();
	}

	pass MeshGround
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MeshUvCut();
	}
}
