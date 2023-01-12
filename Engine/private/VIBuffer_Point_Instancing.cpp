#include "..\public\VIBuffer_Point_Instancing.h"
#include "GameInstance.h"
#include "Shader.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pSpeeds(rhs.m_pSpeeds)
{
}

void CVIBuffer_Point_Instancing::Set_Point_Instancing_Scale(_float3 vScale)
{
	CONTEXT_LOCK;
	// Rect_Instancing 이랑 동일하다.
	
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXPOINTINSTANCING*)SubResource.pData)[i].vPSize.x = vScale.x;
		((VTXPOINTINSTANCING*)SubResource.pData)[i].vPSize.y = vScale.y;
	}

	m_pContext->Unmap(m_pVB, 0);
	
	
}

void CVIBuffer_Point_Instancing::Set_Point_Instancing_Num(_int iInstancingNum)
{
	CONTEXT_LOCK;
	// Rect_Instancing 이랑 동일하다.
	m_iNumInstance = iInstancingNum;
	m_iNumPrimitive = iInstancingNum;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	m_iInstanceStride = sizeof(VTXMATRIX);	// 인스턴싱버퍼는 사이즈를 가지고 있어야한다.

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * iInstancingNum;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iInstancingNum];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_int i = 0; i < iInstancingNum; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);				// 여기 사이즈가 1, 1 이잖아요 인스턴싱된것들이
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(-0.5f +(0.1f *i), 0.1f, -0.5f + (0.1f *i), 1.f);
	}


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;
	Safe_Release(m_pInstanceBuffer);
	m_pInstanceBuffer = nullptr;
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);
	Safe_Delete_Array(pInstanceVertices);
}

void CVIBuffer_Point_Instancing::Set_Point_Instancing_MainTain()
{
	CONTEXT_LOCK;
	m_bIsMaintain = true;
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINTINSTANCING*			pVertices = new VTXPOINTINSTANCING;
	ZeroMemory(pVertices, sizeof(VTXPOINTINSTANCING));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);		// 처음에 다 원점으로 고정
	pVertices->vPSize = _float2(1.f, 1.f);					// 지름이라고 생각하면 편함 실제 전체사이즈의크기

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	Safe_Release(m_pVB);
	m_pVB = nullptr;
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB);
	Safe_Delete_Array(pVertices);

}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pSpeeds = new _double[iNumInstance];

	for (_uint i = 0; i < iNumInstance; ++i)
		m_pSpeeds[i] = rand() % 5 + 1.0;

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;	// 점하나로 찍을거니까 인스턴스 인덱스 버퍼도 한개씩
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOINTINSTANCING);
	m_iNumVertices = 1;
	m_iNumPrimitive = iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINTINSTANCING*			pVertices = new VTXPOINTINSTANCING;
	ZeroMemory(pVertices, sizeof(VTXPOINTINSTANCING));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);		// 처음에 다 원점으로 고정
	pVertices->vPSize = _float2(1.f, 1.f);					// 지름이라고 생각하면 편함 실제 전체사이즈의크기

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

#pragma region INSTANCE_BUFFER			

	m_iInstanceStride = sizeof(VTXMATRIX);	// 인스턴싱버퍼는 사이즈를 가지고 있어야한다.

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);				// 여기 사이즈가 1, 1 이잖아요 인스턴싱된것들이
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.0f, 0.f, 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;

}

HRESULT CVIBuffer_Point_Instancing::Initialize(void * pArg)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));

	if (nullptr != pArg)
		memcpy(&m_Desc, pArg, sizeof(m_Desc));


	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Tick(_double TimeDelta)
{
	CONTEXT_LOCK;
	// Rect_Instancing 이랑 동일하다.
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(0.1f * TimeDelta);

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};
	CONTEXT_LOCK;
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/*m_pContext->DrawIndexed(m_iNumIndices, 0, 0);*/
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instancing::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Point_Buffer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputInt("iFrameCnt", &iPlayOnFrameCnt);
		ImGui::InputFloat("TextureMax_Width_Cnt", &iTextureMax_Width_Cnt);
		ImGui::InputFloat("TextureMax_Height_Cnt", &iTextureMax_Height_Cnt);
		
		if (ImGui::Button("Edit"))
		{
			m_Desc.m_iFrameCnt = iPlayOnFrameCnt;
			m_Desc.m_iTextureMax_Width_Cnt = iTextureMax_Width_Cnt;
			m_Desc.m_iTextureMax_Height_Cnt = iTextureMax_Height_Cnt;
			m_iPlayOnFrameCnt = 0;
		}
	}
}

void CVIBuffer_Point_Instancing::Move_Up_Position(_float4 vPos)
{
	CONTEXT_LOCK;
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = (_float)(vPos.x);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = (_float)(vPos.y);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = (_float)(vPos.z);
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

}

void CVIBuffer_Point_Instancing::Move_Up_Tick(_double TimeDelta, _float fSpeed)
{
	CONTEXT_LOCK;
	// Rect_Instancing 이랑 동일하다.
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (_float)(fSpeed * TimeDelta);
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

}

_bool CVIBuffer_Point_Instancing::UV_Move_Tick(_double TimeDelta)
{

	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_Desc.m_iFrameCnt)
	{
		m_iWidthTextureCnt++;

		if (m_iWidthTextureCnt >= m_Desc.m_iTextureMax_Width_Cnt)
		{
			m_iWidthTextureCnt = 0;
			m_iHeightTextureCnt++;
		}

		if (m_iHeightTextureCnt >= m_Desc.m_iTextureMax_Height_Cnt)
		{
			m_iHeightTextureCnt = 0;
		}

		m_iPlayOnFrameCnt = 0;
	}

	if (m_iHeightTextureCnt == m_Desc.m_iTextureMax_Height_Cnt-1 
		&&   m_iWidthTextureCnt == m_Desc.m_iTextureMax_Width_Cnt - 1)
		return true;

	return false;
}

HRESULT CVIBuffer_Point_Instancing::Set_UV_RawValue(CShader * pShader )
{
	/*if (nullptr == pShader)
		assert(! && "CVIBuffer_Point_Instancing::Set_UV_RawValue");*/

	if (FAILED(pShader->Set_RawValue("g_iUV_Cur_Width_Num", &m_iWidthTextureCnt, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_iUV_Cur_Height_Num", &m_iHeightTextureCnt, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_iUV_Max_Width_Num", &(m_Desc.m_iTextureMax_Width_Cnt), sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_iUV_Max_Height_Num", &(m_Desc.m_iTextureMax_Height_Cnt), sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CVIBuffer_Point_Instancing * CVIBuffer_Point_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{

	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Point_Instancing::Clone(void * pArg)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instancing::Free()
{
	__super::Free();

	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pSpeeds);
	}

	if (m_bIsMaintain == true)
	{
		Safe_Release(m_pVB);
	}

}

