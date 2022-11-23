#include "..\public\Calculator.h"




CCalculator::CCalculator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice,pContext)
{
}

CCalculator::CCalculator(const CCalculator & rhs)
	:CComponent(rhs)
{
}

HRESULT CCalculator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCalculator::Initialize(void * pArg)
{
	return S_OK;
}

CComponent * CCalculator::Clone(void * pArg)
{
	return S_OK;
}

void CCalculator::Free()
{
}
