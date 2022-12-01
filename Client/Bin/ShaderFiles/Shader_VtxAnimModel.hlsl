matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 
	1. 모델의 전체의 뼈를 받아온다 .(x)
	2. 모델 중, 현재 그릴려고 하는 메시에 뼈를 받아온다. (0)
*/

matrix			g_BoneMatrix[256];

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float3		vTangent		: TANGENT;

	/* 현재 정점에게 곱해져야 할 뼈들(최대 4개)의 행렬을 만든다.*/
	uint4		vBlendIndex		: BLENDINDEX;
	float4		vBlendWeight	: BLENDWEIGHT;

};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexUV			: TEXCOORD0;
	float4		vTangent		: TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	float	fWeightW = 1.f - (In.vBlendWeight.x +
									In.vBlendWeight.y + In.vBlendWeight.z);
	// 왜 이렇게 구할까? weight xyzw의 합은 1이다.
	// 그러나 메쉬에 뼈가 없을수도 있으므로 강제로 뼈의 개수를 1개로 만들어주는것이다.
	// 뼈가 없을경우 항등행렬을 던져야 한다.

	matrix		BoneMatrix = g_BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrix[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal	  = mul(float4(In.vNormal, 0.f), BoneMatrix);


	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = mul(vNormal, g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vTangent = (vector)0.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTangent : TANGENT;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a <= 0.3f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
