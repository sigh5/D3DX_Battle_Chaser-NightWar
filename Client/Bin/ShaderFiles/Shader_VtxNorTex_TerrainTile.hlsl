
#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;


/* ������ */
vector			g_vLightDir;
vector			g_vLightPos;		// For_PointLight
float			g_fRange;			// For_PointLight
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;


/* �������� */
int				g_iDiffuseTextureIndex = 0; 
int				g_iFillTerTextureIndex = 1;

texture2D		g_DiffuseTexture[40];

//texture2D		g_DiffuseTexture;
//texture2D		g_DestTexture;


vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);



/* ���� ���̵�*/
texture2D		g_FilterTexture[3];


texture2D		g_FilterCanvasTexture;


texture2D		g_BrushTexture;
vector			g_vBrushPos;
float			g_fBrushRange = 5.f;



struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;

};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2	    vTexUV			: TEXCOORD0;
	float4		vWolrdPos		: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	// �׻� ���彺���̽����� ����ϴ��� ���ÿ��� ����ϴ��� Ȯ���ؾ��Ѵ�.
	Out.vWolrdPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2	    vTexUV			: TEXCOORD0;
	float4		vWorldPos		: TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/*vector vDiffuseArr[35];
	for (int i = 0; i<35; i++)
		vDiffuseArr[i] = g_DiffuseTexture[i].Sample(LinearSampler, In.vTexUV * 30.f);*/

	//vector vSourDiffuse = vDiffuseArr[g_iDiffuseTextureIndex];
	//vector vDestDiffuse = vDiffuseArr[g_iFillTerTextureIndex];

	vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV * 30.f);
	vector vDestDiffuse = g_DiffuseTexture[5].Sample(LinearSampler, In.vTexUV * 30.f);
	
	//vector vDestDiffuse2 = g_DiffuseTexture[27].Sample(LinearSampler, In.vTexUV * 30.f);



	vector		vFilter = g_FilterCanvasTexture.Sample(LinearSampler, In.vTexUV);
	//vector		vFilter1 = g_FilterTexture[1].Sample(LinearSampler, In.vTexUV);


	vector		vBrush = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV;

		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}
	

	vector		vMtrlDiffuse = vSourDiffuse * vFilter.r +
		vDestDiffuse * (1.f - vFilter.r);//+    vBrush;

	
	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse) ;

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	/* �� ���� : ���� �ݻ纤��, ������ �ٶ󺸴� �ü����� */
	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}


PS_OUT PS_MAIN_Real(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/*vector vDiffuseArr[35];
	for (int i = 0; i<35; i++)
	vDiffuseArr[i] = g_DiffuseTexture[i].Sample(LinearSampler, In.vTexUV * 30.f);*/

	//vector vSourDiffuse = vDiffuseArr[g_iDiffuseTextureIndex];
	//vector vDestDiffuse = vDiffuseArr[g_iFillTerTextureIndex];

	vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV * 30.f);
	vector vDestDiffuse0 = g_DiffuseTexture[20].Sample(LinearSampler, In.vTexUV * 30.f);

	vector vDestDiffuse1 = g_DiffuseTexture[27].Sample(LinearSampler, In.vTexUV * 30.f);
	vector vDestDiffuse2 = g_DiffuseTexture[5].Sample(LinearSampler, In.vTexUV * 30.f);


	vector		vFilter = g_FilterTexture[0].Sample(LinearSampler, In.vTexUV);
	vector		vFilter1 = g_FilterTexture[1].Sample(LinearSampler, In.vTexUV);
	vector		vFilter2 = g_FilterTexture[2].Sample(LinearSampler, In.vTexUV);

	vector		vBrush = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV;

		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}

	vector		vMtrlDiffuse;
	vector		vSour = (vSourDiffuse* vFilter.r * vFilter1.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r);
	vector		vDest = (vSourDiffuse* vFilter1.r * vFilter2.r) + vDestDiffuse1 * (1.f - vFilter1.r) + vDestDiffuse2 * (1.f - vFilter2.r);
	vector		vTemp = (vSourDiffuse* vFilter.r * vFilter2.r) + vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse2 * (1.f - vFilter2.r);


	vMtrlDiffuse = (vSourDiffuse * vFilter.r * vFilter1.r )		//* vFilter2.r
		+ vDestDiffuse0 * (1.f - vFilter.r) + vDestDiffuse1 * (1.f - vFilter1.r); //+vDestDiffuse2 * (1.f - vFilter2.r);
	
/*	if (vDest.a > 1.f)
	{
		vMtrlDiffuse = vDestDiffuse2;
	}
	else*/ if (vSour.a > 1.f)
	{
		vMtrlDiffuse = vDestDiffuse1;
	}
	//else if (vTemp.a > 1.f)
	//{
	//	vMtrlDiffuse = vDestDiffuse2;
	//}



	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse);

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	/* �� ���� : ���� �ݻ纤��, ������ �ٶ󺸴� �ü����� */
	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	
	return Out;
}


technique11 DefaultTechnique
{
	pass Terrain_Diffuse
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


	pass Terrain_Diffuse_MultiFilter
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Real();
	}

}