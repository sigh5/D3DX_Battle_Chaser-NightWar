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
texture2D		g_DiffuseTexture;
vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* ���� ���̵�*/
texture2D		g_BrushTexture;
vector			g_vBrushPos;
float			g_fBrushRange = 5.f;

sampler				LinearSampler = sampler_state
{
	filter = MIN_MAG_MIP_LINEAR;		//D3D11_SAMPLER_DESC ����
	AddressU = WRAP;	// �⺻������ �������� CLamp�� �⺻�̴�.
	AddressV = WRAP;
};

sampler				PointSampler = sampler_state
{
	filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};


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

struct VS_OUT_PHONG
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};


VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
	VS_OUT_PHONG Out = (VS_OUT_PHONG)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition,1.f), g_WorldMatrix);
	Out.vNormal = mul(float4(In.vNormal,0.f), g_WorldMatrix);
	
	return Out;
}


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

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);

	vector		vBrush = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV;

		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}

	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse) + vBrush;

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

struct PS_IN_PHONG
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV* 30.f);
	vector	vDiffuse = g_vLightDiffuse * vMtrlDiffuse;

	float	fShade = saturate(dot(normalize(g_vLightDir)*-1.f,
		normalize(In.vNormal)));


	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(saturate(dot(normalize(vReflect)*-1.f,
		normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}


PS_OUT PS_MAIN_POINT(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);

	vector		vDiffuse = g_vLightDiffuse * vMtrlDiffuse;

	vector		vLightDir = In.vWorldPos - g_vLightPos;

	/* �� �ȼ��� ���������� �Ÿ� .*/
	float		fDistance = length(vLightDir);


	/* �� ����¿� ���谪*/
	float		fAtt = saturate((g_fRange - fDistance) / g_fRange);

	float		fShade = saturate(dot(normalize(vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	/* �� ���� : ���� �ݻ纤��, ������ �ٶ󺸴� �ü����� */
	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	Out.vColor = (vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular)) * fAtt;

	return Out;
}


technique11 DefaultTechnique
{

	pass Terrain_Diffuse
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}


	pass Terrain_Phong
	{
		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG();
	}


	pass Point
	{
		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}


}