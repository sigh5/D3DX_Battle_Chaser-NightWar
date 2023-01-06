#include "..\public\VIBuffer_Point.h"
#include "GameInstance.h"
#include "Shader.h"


CVIBuffer_Point::CVIBuffer_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point & rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXPOINTINSTANCING);
	m_iNumVertices = 1;
	m_iNumPrimitive = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINTINSTANCING*			pVertices = new VTXPOINTINSTANCING;
	ZeroMemory(pVertices, sizeof(VTXPOINTINSTANCING));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);		// 처음에 다 원점으로 고정
	pVertices->vPSize = _float2(4.f, 4.f);

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

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion
	return S_OK;

}

HRESULT CVIBuffer_Point::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Point::Imgui_RenderProperty()
{
	ImGui::InputInt("iFrameCnt", &iPlayOnFrameCnt);
	ImGui::InputInt("TextureMax_Width_Cnt", &iTextureMax_Width_Cnt);
	ImGui::InputInt("TextureMax_Height_Cnt", &iTextureMax_Height_Cnt);
	ImGui::InputFloat("WidthRatio", &fWidthRatio);
	ImGui::InputFloat("HeightRatio", &fHeightRatio);

	if (ImGui::Button("Edit"))
	{
		m_iFrameCnt = iPlayOnFrameCnt;
		m_iTextureMax_Width_Cnt = iTextureMax_Width_Cnt;
		m_iTextureMax_Height_Cnt = iTextureMax_Height_Cnt;
		m_fWidthRatio = fWidthRatio;
		m_fHeightRatio = fHeightRatio;
		m_iPlayOnFrameCnt = 0;
	}
}

void CVIBuffer_Point::UV_Move_Tick(_double TimeDelta)
{
	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_iFrameCnt)
	{
		m_iWidthTextureCnt++;

		if (m_iWidthTextureCnt >= m_iTextureMax_Width_Cnt)
		{
			m_iWidthTextureCnt = 0;
			m_iHeightTextureCnt++;
			m_fOldX = m_fCurX;
		}

		if (m_iHeightTextureCnt >= m_iTextureMax_Height_Cnt)
		{
			m_iHeightTextureCnt = 0;
			m_fOldY = m_fCurY;
		}

		m_iPlayOnFrameCnt = 0;
		m_fCurX = (m_iWidthTextureCnt % m_iTextureMax_Width_Cnt) *m_fWidthRatio;
		m_fCurY = (m_iHeightTextureCnt % m_iTextureMax_Height_Cnt) *m_fHeightRatio;
	}

}

HRESULT CVIBuffer_Point::Set_UV_RawValue(CShader * pShader)
{
	if (m_iPlayOnFrameCnt == 0 || pShader == nullptr)
		return S_OK;

	if (FAILED(pShader->Set_RawValue("g_UV_XCurRatio", &m_fCurX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_UV_YCurRatio", &m_fCurY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_UV_YSub", &m_fWidthRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_UV_XSub", &m_fHeightRatio, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CVIBuffer_Point * CVIBuffer_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Point*		pInstance = new CVIBuffer_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Point::Clone(void * pArg)
{
	CVIBuffer_Point*		pInstance = new CVIBuffer_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point::Free()
{
	__super::Free();
}