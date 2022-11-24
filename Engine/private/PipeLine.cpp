#include "..\public\PipeLine.h"

#include "GameInstance.h"
IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{

}

HRESULT CPipeLine::Ready_PipeLine()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	Set_Transform(CPipeLine::D3DTS_ORTH, XMMatrixOrthographicLH((_float)pGameInstance->Get_GraphicDesc().iViewportSizeX, (_float)pGameInstance->Get_GraphicDesc().iViewportSizeY, 0.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrices[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATE eState) const
{
	return m_TransformMatrices[eState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrices_Inverse[eState]);
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_VIEW],
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_VIEW])));
	XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_PROJ],
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_PROJ])));

	

	memcpy(&m_vCamPosition, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}
