#include "..\public\Collider.h"
#include "DebugDraw.h"

#include "PipeLine.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	COLLIDERDESC		ColliderDesc;
	memcpy(&ColliderDesc, pArg, sizeof(COLLIDERDESC));

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB_Original->Transform(*m_pAABB_Original,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		m_pAABB = new BoundingBox(*m_pAABB_Original);

		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(ColliderDesc.vSize.x * 0.5f, ColliderDesc.vSize.y * 0.5f, ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB_Original->Transform(*m_pOBB_Original,
			// XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *			
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);

		m_pSphere_Original->Transform(*m_pSphere_Original,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));

		m_pSphere = new BoundingSphere(*m_pSphere_Original);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
		break;
	}
}

_bool CCollider::Collision(CCollider * pTargetCollider)
{
	m_isColl = false;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_OBB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_SPHERE:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pSphere);
		break;
	}

	return m_isColl;
}

_bool CCollider::Collision_Mouse(HWND hWnd)
{
	m_isColl = false;


	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;

	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));

	RELEASE_INSTANCE(CGameInstance);

	_float4 vCenter,vExtension;
	
	vCenter.x = m_pAABB->Center.x;
	vCenter.y = m_pAABB->Center.y;
	vCenter.z = m_pAABB->Center.z;
	vCenter.w = 1.f;
	
	vExtension.x = m_pAABB->Extents.x;
	vExtension.y = m_pAABB->Extents.y;
	vExtension.z = m_pAABB->Extents.z;
	vExtension.w = 0;

	_float4	dwVtxIdx[8]{};
	_float fDist;

	dwVtxIdx[0] = { vCenter.x - vExtension.x,vCenter.y + vExtension.y,vCenter.z - vExtension.z,1.f };
	dwVtxIdx[1] = { vCenter.x + vExtension.x,vCenter.y + vExtension.y,vCenter.z - vExtension.z,1.f };
	dwVtxIdx[2] = { vCenter.x + vExtension.x,vCenter.y - vExtension.y,vCenter.z - vExtension.z,1.f };
	dwVtxIdx[3] = { vCenter.x - vExtension.x,vCenter.y - vExtension.y,vCenter.z - vExtension.z,1.f };
	dwVtxIdx[4] = { vCenter.x - vExtension.x,vCenter.y + vExtension.y,vCenter.z + vExtension.z,1.f };
	dwVtxIdx[5] = { vCenter.x + vExtension.x,vCenter.y + vExtension.y,vCenter.z + vExtension.z,1.f };
	dwVtxIdx[6] = { vCenter.x + vExtension.x,vCenter.y - vExtension.y,vCenter.z + vExtension.z,1.f };
	dwVtxIdx[7] = { vCenter.x - vExtension.x,vCenter.y - vExtension.y,vCenter.z + vExtension.z,1.f };


	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[1]),
		XMLoadFloat4(&dwVtxIdx[5]),
		XMLoadFloat4(&dwVtxIdx[6]),
		fDist))
	{
		return true;
	}
	
	
	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[1]),
		XMLoadFloat4(&dwVtxIdx[6]),
		XMLoadFloat4(&dwVtxIdx[2]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[4]),
		XMLoadFloat4(&dwVtxIdx[0]),
		XMLoadFloat4(&dwVtxIdx[3]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[4]),
		XMLoadFloat4(&dwVtxIdx[3]),
		XMLoadFloat4(&dwVtxIdx[7]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[4]),
		XMLoadFloat4(&dwVtxIdx[5]),
		XMLoadFloat4(&dwVtxIdx[1]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[4]),
		XMLoadFloat4(&dwVtxIdx[1]),
		XMLoadFloat4(&dwVtxIdx[0]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[3]),
		XMLoadFloat4(&dwVtxIdx[2]),
		XMLoadFloat4(&dwVtxIdx[6]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[7]),
		XMLoadFloat4(&dwVtxIdx[6]),
		XMLoadFloat4(&dwVtxIdx[5]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[7]),
		XMLoadFloat4(&dwVtxIdx[5]),
		XMLoadFloat4(&dwVtxIdx[4]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[0]),
		XMLoadFloat4(&dwVtxIdx[1]),
		XMLoadFloat4(&dwVtxIdx[2]),
		fDist))
	{
		return true;
	}

	if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
		XMVector3Normalize(XMLoadFloat4(&vRayDir)),
		XMLoadFloat4(&dwVtxIdx[0]),
		XMLoadFloat4(&dwVtxIdx[2]),
		XMLoadFloat4(&dwVtxIdx[3]),
		fDist))
	{
		return true;
	}

	return false;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_vColor = m_isColl == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix			ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);

#ifdef _DEBUG

	Safe_Release(m_pInputLayout);

	if (false == m_bClone)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif // _DEBUG

}
