
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vCamPosition;

texture2D		g_DiffuseTexture;
texture2D		g_HeightTexture;
texture2D		g_UVTexture;

float			g_TimeDelta;
float			g_UVSpeed;

/* 빛정보 */
vector			g_vLightDir;
vector			g_vLightPos;		// For_PointLight
float			g_fRange;			// For_PointLight
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* 재질정보 */
vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);


struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent	: TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTangent : TANGENT;
	float4		vWolrdPos		: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	float2 vHeight = g_HeightTexture.SampleLevel(LinearSampler, In.vPosition.xy, 0);
	In.vPosition.y = vHeight.x;

	//float cosTime = g_WaveHeight * cos(g_Time * g_Speed + In.vTexUV.x * g_WaveFrequency);
	//In.vPosition.y += cosTime;

	float2		temp = g_UVTexture.SampleLevel(LinearSampler, In.vPosition.xy, 0);
	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV + float2(g_TimeDelta * g_UVSpeed, 0);
	Out.vTangent = (vector)0.f;
	Out.vWolrdPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	return Out;

}

VS_OUT VS_MAIN_WaterFall(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	float2		temp = g_UVTexture.SampleLevel(LinearSampler, In.vPosition.xy, 0);
	matrix      matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV + float2(g_TimeDelta * g_UVSpeed, 0);
	Out.vTangent = (vector)0.f;
	Out.vWolrdPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	return Out;

}






struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal   : NORMAL;
	float2		vTexUV    : TEXCOORD0;
	float4		vTangent  : TANGENT;
	float4		vWolrdPos	: TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse);

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWolrdPos - g_vCamPosition;

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);


	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);


	//Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	
	return Out;
}


technique11 DefaultTechnique
{

	pass Water_Diffuse
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

	pass Water_Fall
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_WaterFall();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();

	}



}
