
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


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_WIRE_Model
{
	FillMode = WIREFRAME;
	CullMode = Back;
	FrontCounterClockwise = false;
};


RasterizerState RS_CW
{
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_NONE
{
	CullMode = None;
	FrontCounterClockwise = true;
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

BlendState BS_One			// ���� �׸��� �ָ��� ������ ������ ���°�
{
	BlendEnable[0] = true;
SrcBlend = ONE;
DestBlend = ONE;
BlendOp = Add;
};


