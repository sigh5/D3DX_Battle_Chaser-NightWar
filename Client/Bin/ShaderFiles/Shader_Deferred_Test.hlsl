matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;

matrix			g_BoneMatrices[256];

texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

texture2D		g_BGTexture;


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
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* 현재 정점에게 곱해져야할 뼈들(최대 4개)의 행렬을 만든다. */
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTangent : TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
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

	float4		vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse* vShade + vSpecular;

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_BG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_BGTexture.Sample(LinearSampler, In.vTexUV);
	
	Out.vColor = vDiffuse;

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}



RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};


RasterizerState RS_CW
{
	CullMode = Front;
	FrontCounterClockwise = false;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DS_ZEnable_ZWriteEnable_FALSE
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;


	//bool BlendEnable;
	//D3D11_BLEND SrcBlend;
	//D3D11_BLEND DestBlend;
	//D3D11_BLEND_OP BlendOp;
	//D3D11_BLEND SrcBlendAlpha;
	//D3D11_BLEND DestBlendAlpha;
	//D3D11_BLEND_OP BlendOpAlpha;
	//UINT8 RenderTargetWriteMask;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};



technique11 DefaultTechnique
{
	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass BG
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BG();
	}

}
