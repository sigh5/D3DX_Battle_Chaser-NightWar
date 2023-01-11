#include "Shader_Client_Defines.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

vector			g_vCamPosition;		// ������ �������� �������ϱ����ؼ�

float			g_Ratio = 1.f;

BlendState			AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = src_alpha;
	DestBlend = inv_Src_Alpha;
	BlendOp = Add;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_GS_OUT
{
	float3		vPosition : POSITION;
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

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;		// ���� ������Ʈ�� ���̴� ������ �����Ͷ���� ���������͸� �ȼ� ���̴��� ����
	float2		vTexUV : TEXCOORD0;
};

//[maxvertexcount(6)]		// ������ ���ڰ� ��������� 20�� ���ϰ� ������Ʈ���� ������ �����ϱ⿡ ����,
//void	GS_MAIN(point GS_IN In[3], inout TriangleStream<GS_OUT> Vertices)
//{
//	//  GS_IN In[N] N�� ���ڴ�
//	// TOPOLOGY�� �����̸� 2�� ����Ʈ�� 1�� Ʈ���̾ޱ۸���Ʈ�� 3�� 
//
//	//GS_OUT		Out[4];	// �簢���� ����Ŵϱ� �ε����� 4���ʿ�
//
//	//					/* ��������� ���� Right,Up,Look �� ���Ѵ� */
//	//float3		vLook = g_vCamPosition.xyz - In[0].vPosition;		//ī�޶� ���ؿ°��� ������ó�� ������� 
//	//float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * 0.5f;
//	//float3		vUp = normalize(cross(vLook, vRight))  * 0.5f;
//
//	//matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
//
//	//float3		vPosition;
//
//
//	//Out[0].vTexUVOrigin = float2(0.f, 0.f);
//	//Out[1].vTexUVOrigin = float2(1.f, 0.f);
//	//Out[2].vTexUVOrigin = float2(1.f, 1.f);
//	//Out[3].vTexUVOrigin = float2(0.f, 1.f);
//
//
//
//
//	//vPosition = In[0].vPosition + vRight + vUp;
//	//Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
//	//Out[0].vTexUV = float2(1.f / g_iUV_Max_Width_Num * (g_iUV_Cur_Width_Num),
//	//	(1.f) / g_iUV_Max_Height_Num * (g_iUV_Cur_Height_Num));
//
//	//vPosition = In[0].vPosition - vRight + vUp;
//	//Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
//	//Out[1].vTexUV = float2(1.f / g_iUV_Max_Width_Num *(g_iUV_Cur_Width_Num + 1.f),
//	//	(1.f) / g_iUV_Max_Height_Num* (g_iUV_Cur_Height_Num));
//
//	//vPosition = In[0].vPosition - vRight - vUp;
//	//Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
//	//Out[2].vTexUV = float2(1.f / g_iUV_Max_Width_Num* (g_iUV_Cur_Width_Num + 1.f),
//	//	(1.f) / g_iUV_Max_Height_Num * (g_iUV_Cur_Height_Num + 1.f));
//
//	//vPosition = In[0].vPosition + vRight - vUp;
//	//Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
//	//Out[3].vTexUV = float2(1.f / g_iUV_Max_Width_Num * g_iUV_Cur_Width_Num,
//	//	(1.f) / g_iUV_Max_Height_Num* (g_iUV_Cur_Height_Num + 1.f));
//
//
//
//	//Vertices.Append(Out[0]);
//	//Vertices.Append(Out[1]);
//	//Vertices.Append(Out[2]);
//	//Vertices.RestartStrip();		// �̰� �Ƚ��ָ� ���� ��Ʈ��ó���׷��� 
//	//								//Ʈ���̾ޱ۸���Ʈ�� ������ �ѹ� ��������Ѵ�.
//
//	//Vertices.Append(Out[0]);
//	//Vertices.Append(Out[2]);
//	//Vertices.Append(Out[3]);
//	//Vertices.RestartStrip();
//}



struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/*SV_TARGET0 : ��� ������ ������ �ȼ��̴�. AND 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	return Out;
}

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	return Out;
}


technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}


	pass AlPhaBlend
	{

		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
	}

	pass Rect_2
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
