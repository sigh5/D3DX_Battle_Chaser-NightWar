
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;

/* 빛정보 */
vector			g_vLightDir;
vector			g_vLightPos;		// For_PointLight
float			g_fRange;			// For_PointLight
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;


/* 재질정보 */
texture2D		g_DiffuseTexture;
vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 지형 셰이딩*/
texture2D		g_BrushTexture;


struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition	: SV_POSITION;
	float		fShade		: COLORD0;
	float	    fSpecular	: COLORD1;
	float2		vTexUV		: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	// 항상 월드스페이스에서 계산하는지 로컬에서 계산하는지 확인해야한다.
	vector	vWorldPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector	vWorldNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(vWorldNormal)));		// 노말라이즈 한값을 넘겨주긴 하지만 혹시 모르니 
					// 그리고 조명계산은 cos값이지만 음수가 나오면안되므로 0이상의 값을 출력
					// 을보장해주는 saturate(Max 랑 비슷한 기능을한다.) 를 써야한다.

	vector	vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector	vLook	 = vWorldPosition - g_vCamPosition;

	/* 두 벡터 : 빛의 반사벡터, 정점을 바라보는 시선벡터 */
	/* pow 를 이용하여 1과 가까울때만 반사값을 크게만들고 나머지는 누워있게만든다.*/
	Out.fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	return Out;
}

struct PS_IN
{
	float4		vPosition	 : SV_POSITION;
	float		fShade		 : COLORD0;
	float	    fSpecular	 : COLORD1;
	float2		vTexUV		 : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);

	vector		vDiffuse = g_vLightDiffuse * vMtrlDiffuse;

	Out.vColor = vDiffuse * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ In.fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

technique11 DefaultTechnique
{
	pass Terrain
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
}
