
#include "Shader_Client_Defines.h"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector				g_vCamPosition;		// ������ �������� �������ϱ����ؼ�

texture2D			g_Texture;
texture2D			g_ColorTexture;
texture2D			g_ColorTexture2;

vector				g_WorldTrailPos[4];

float				g_fUV_X_Ratio;
float				g_fFixRatio;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;		//PSIZE ��� �ø�ƽ�� �����Ѵ�.

	row_major float4x4	Matrix : WORLD;		//Row_major �ڷ����� ����
};

struct VS_OUT
{
	float3		vPosition : POSITION;		//���� �ȼ� ���̴������� �� �ִ� �����Ͱ� �ƴ϶� SV_ �ø�ƽ�� �����Ѵ�.
	float2		vPSize : PSIZE;
};


/* ������ �߰������� ������ �� �ִ� ��Ȳ�� �غ��صд�.*/
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vPSize = In.vPSize;

	return Out;
}


struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;		// ���� ������Ʈ�� ���̴� ������ �����Ͷ���� ���������͸� �ȼ� ���̴��� ����
	float2		vTexUV : TEXCOORD0;
	
};

[maxvertexcount(6)]		// ������ ���ڰ� ��������� 20�� ���ϰ� ������Ʈ���� ������ �����ϱ⿡ ����,
void	GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];	// �簢���� ����Ŵϱ� �ε����� 4���ʿ�

	//float3		vLook = g_vCamPosition.xyz - g_FirstPos.xyz;		//ī�޶� ���ؿ°��� ������ó�� ������� 
	//float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	//float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;


	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	float3		vPosition;

	vPosition = g_WorldTrailPos[0];//+ vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(g_fUV_X_Ratio , 0.f) ;

	vPosition = g_WorldTrailPos[3];//+ vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(g_fUV_X_Ratio , 1.f) ;


	vPosition = g_WorldTrailPos[1];//- vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(g_fUV_X_Ratio  + g_fFixRatio, 0.f) ;

	vPosition = g_WorldTrailPos[2];//+ vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(g_fUV_X_Ratio  + g_fFixRatio, 1.f);


	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();		// �̰� �Ƚ��ָ� ���� ��Ʈ��ó���׷��� 
									//Ʈ���̾ޱ۸���Ʈ�� ������ �ѹ� ��������Ѵ�.

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
	
}

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


	float4	TextureColor0, TextureColor1, TexturColor ,TempVec;
	TexturColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	TextureColor0 = g_ColorTexture.Sample(LinearSampler, In.vTexUV);
	TextureColor1 = g_ColorTexture2.Sample(LinearSampler, In.vTexUV);

	if (TextureColor0.r < TextureColor1.r)
		TempVec = TextureColor1;
	else
		TempVec = TextureColor0;

	Out.vColor = saturate(TexturColor * TempVec);


	if (Out.vColor.a <= 0.5f)
		discard;
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_NONE);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}


}
