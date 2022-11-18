matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

//sampler			DefaultSampler = sampler_state
//{
//
//};

sampler				LinearSampler = sampler_state
{
	filter = MIN_MAG_MIP_LINEAR;		//D3D11_SAMPLER_DESC 참고
	AddressU = WRAP;	// 기본적으로 안적으면 CLamp가 기본이다.
	AddressV = WRAP;
};

//sampler				PointSampler = sampler_state
//{
//	fileter = MIN_MAG_MIP_POINT;
//	AddressU = CLAMP;
//	AddressV = CLAMP;
//};


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal   : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV*2.f);
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
