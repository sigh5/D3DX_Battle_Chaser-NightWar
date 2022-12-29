#include "..\public\RcEffect.h"





CRcEffect::CRcEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice,pContext), m_iPlayOnFrameCnt(0), m_iWidthTextureCnt(0), m_iHeightTextureCnt(0)
{
}

CRcEffect::CRcEffect(const CRcEffect & rhs)
{

}


HRESULT CRcEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRcEffect::Initialize(void * pArg)
{
	ZeroMemory(&m_EffectDesc, sizeof(m_EffectDesc));

	if (pArg != nullptr)
		memcpy(&m_EffectDesc, pArg, sizeof(m_EffectDesc));




	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXUVPARTICLE);
	m_iNumVertices = 4;
	m_iNumPrimitive = 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXUVPARTICLE*			pVertices = new VTXUVPARTICLE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXUVPARTICLE));

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[0].vTexUV = m_EffectDesc._Uv_0;

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[1].vTexUV = m_EffectDesc._Uv_1;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[2].vTexUV = m_EffectDesc._Uv_2;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[3].vTexUV = m_EffectDesc._Uv_3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CRcEffect::Set_Texture(CTexture * pTexture)
{
	m_pTexture = pTexture;
}

void CRcEffect::Set_TextureOption(_uint _framecnt, _uint _texturecntw, _uint _texturecnth)
{
	m_iFrameCnt = _framecnt;
	m_iTextureCnt_W = _texturecntw;
	m_iTextureCnt_H = _texturecnth;
}

_bool CRcEffect::Check_TextureCnt(_uint _widthtexturecnt, _uint heighttexturecnt)
{
	if (m_iWidthTextureCnt == _widthtexturecnt && m_iHeightTextureCnt == heighttexturecnt)
		return true;
	else
		return false;
}

void CRcEffect::Check_Alpha(_bool _TrueIsAlphaOn)
{
	m_bIsAlpha = _TrueIsAlphaOn;
}

const _uint & CRcEffect::Get_WidthTextureCnt(void)
{
	return m_iWidthTextureCnt;
}

const _uint & CRcEffect::Get_HeightTextureCnt(void)
{
	return m_iHeightTextureCnt;
}

CRcEffect * CRcEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CComponent * CRcEffect::Clone(void * pArg)
{
	return nullptr;
}

void CRcEffect::Free()
{
	__super::Free();
}
